/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RemoteQueryService.hpp"

#include "CacheImpl.hpp"
#include "CqServiceVsdStats.hpp"
#include "RemoteQuery.hpp"
#include "ThinClientPoolDM.hpp"
#include "UserAttributes.hpp"
#include "statistics/StatisticsManager.hpp"

namespace apache {
namespace geode {
namespace client {

RemoteQueryService::RemoteQueryService(CacheImpl* cache,
                                       ThinClientPoolDM* poolDM)
    : m_invalid(true),
      m_cqService(nullptr),
      m_statisticsFactory(
          cache->getStatisticsManager().getStatisticsFactory()) {
  if (poolDM) {
    m_tccdm = poolDM;
  } else {
    m_tccdm =
        new ThinClientCacheDistributionManager(cache->tcrConnectionManager());
  }
  LOGFINEST("Initialized m_tccdm");
}

void RemoteQueryService::init() {
  boost::unique_lock<boost::shared_mutex> lock(mutex_, boost::defer_lock);
  while (!lock.try_lock()) {
    if (m_invalid) {
      break;
    }
  }

  if (m_invalid) {
    LOGFINEST("RemoteQueryService::init: initializing TCCDM");
    if (dynamic_cast<ThinClientCacheDistributionManager*>(m_tccdm)) {
      m_tccdm->init();
    }
    m_invalid = false;
    LOGFINEST("RemoteQueryService::init: done initialization");
  }
}

void RemoteQueryService::close() {
  LOGFINEST("RemoteQueryService::close: starting close");
  boost::unique_lock<boost::shared_mutex> lock(mutex_, boost::defer_lock);
  while (!lock.try_lock()) {
    if (m_invalid) {
      break;
    }
  }

  if (m_cqService != nullptr) {
    LOGFINEST("RemoteQueryService::close: starting CQ service close");
    m_cqService->closeCqService();
    m_cqService = nullptr;
    LOGFINEST("RemoteQueryService::close: completed CQ service close");
  }

  if (dynamic_cast<ThinClientCacheDistributionManager*>(m_tccdm)) {
    if (!m_invalid) {
      LOGFINEST("RemoteQueryService::close: destroying DM");
      m_tccdm->destroy();
    }
    _GEODE_SAFE_DELETE(m_tccdm);
    m_invalid = true;
  }

  if (!m_CqPoolsConnected.empty()) {
    m_CqPoolsConnected.clear();
  }

  LOGFINEST("RemoteQueryService::close: completed");
}

std::shared_ptr<Query> RemoteQueryService::newQuery(std::string querystring) {
  LOGDEBUG("RemoteQueryService::newQuery: multiuserMode = %d ",
           m_tccdm->isMultiUserMode());

  return doIfNotDestroyed<std::shared_ptr<Query>>([&]() {
    LOGDEBUG("RemoteQueryService: creating a new query: " + querystring);
    if (m_tccdm->isMultiUserMode()) {
      return std::make_shared<RemoteQuery>(
          querystring, shared_from_this(), m_tccdm,
          UserAttributes::threadLocalUserAttributes->getAuthenticatedView());
    } else {
      return std::make_shared<RemoteQuery>(querystring, shared_from_this(),
                                           m_tccdm);
    }
  });
}

/**
 * execute all cqs on the endpoint after failover
 */
GfErrType RemoteQueryService::executeAllCqs(TcrEndpoint* endpoint) {
  try {
    return doIfNotDestroyed<GfErrType>([&]() {
      if (m_cqService) {
        LOGFINE(
            "RemoteQueryService: execute all cqs after failover to endpoint[" +
            endpoint->name() + "]");
        return m_cqService->executeAllClientCqs(endpoint);
      } else {
        LOGFINE(
            "RemoteQueryService: no cq to execute after failover to endpoint[" +
            endpoint->name() + "]");
        return GF_NOERR;
      }
    });
  } catch (const CacheClosedException&) {
    LOGFINE("QueryService::executeAllCqs(endpoint): Not initialized.");
    return GF_NOERR;
  }
}

void RemoteQueryService::executeAllCqs(bool failover) {
  try {
    doIfNotDestroyed<void>([&]() {
      if (m_cqService) {
        LOGFINE("RemoteQueryService: execute all cqs after failover");
        m_cqService->executeAllClientCqs(failover);
      } else {
        LOGFINE("RemoteQueryService: no cq to execute after failover");
      }
    });
  } catch (const CacheClosedException&) {
    LOGFINE("QueryService::executeAllCqs: Not initialized.");
  }
}

std::shared_ptr<CqQuery> RemoteQueryService::newCq(
    std::string querystr, const std::shared_ptr<CqAttributes>& cqAttr,
    bool isDurable) {
  return doIfNotDestroyed<std::shared_ptr<CqQuery>>([&]() {
    initCqService();
    // use query string as name for now
    auto name = "_default" + querystr;
    return m_cqService->newCq(name, querystr, cqAttr, isDurable);
  });
}

std::shared_ptr<CqQuery> RemoteQueryService::newCq(
    std::string name, std::string querystr,
    const std::shared_ptr<CqAttributes>& cqAttr, bool isDurable) {
  return doIfNotDestroyed<std::shared_ptr<CqQuery>>([&]() {
    initCqService();
    return m_cqService->newCq(name, querystr, cqAttr, isDurable);
  });
}

void RemoteQueryService::closeCqs() {
  try {
    doIfNotDestroyed<void>([&]() {
      // If cqService has not started, then no cq exists
      if (m_cqService != nullptr) {
        m_cqService->closeAllCqs();
      }
    });
  } catch (const CacheClosedException&) {
    LOGFINE("QueryService::closeCqs: Cache has been closed.");
  }
}

CqService::query_container_type RemoteQueryService::getCqs() const {
  return doIfNotDestroyed<CqService::query_container_type>([&]() {
    // If cqService has not started, then no cq exists
    CqService::query_container_type vec;
    if (m_cqService) {
      vec = m_cqService->getAllCqs();
    }
    return vec;
  });
}

std::shared_ptr<CqQuery> RemoteQueryService::getCq(
    const std::string& name) const {
  return doIfNotDestroyed<std::shared_ptr<CqQuery>>([&]() {
    // If cqService has not started, then no cq exists
    if (m_cqService) {
      return m_cqService->getCq(name);
    }

    return std::shared_ptr<CqQuery>{};
  });
}

void RemoteQueryService::executeCqs() {
  doIfNotDestroyed<void>([&]() {
    // If cqService has not started, then no cq exists
    if (m_cqService != nullptr) {
      m_cqService->executeAllClientCqs();
    }
  });
}

void RemoteQueryService::stopCqs() {
  try {
    doIfNotDestroyed<void>([&]() {
      // If cqService has not started, then no cq exists
      if (m_cqService != nullptr) {
        m_cqService->stopAllClientCqs();
      }
    });
  } catch (const CacheClosedException&) {
    LOGFINE("QueryService::stopCqs: Cache has been closed.");
  }
}

std::shared_ptr<CqServiceStatistics>
RemoteQueryService::getCqServiceStatistics() const {
  return doIfNotDestroyed<std::shared_ptr<CqServiceStatistics>>([&]() {
    // If cqService has not started, then no cq exists
    if (m_cqService) {
      return m_cqService->getCqServiceStatistics();
    }

    return std::shared_ptr<CqServiceStatistics>{};
  });
}

void RemoteQueryService::receiveNotification(TcrMessage* msg) {
  try {
    if (doIfNotDestroyed<bool>([&]() {
          // If cqService has not started, then no cq exists
          if (!m_cqService) {
            return false;
          }
          if (!m_cqService->checkAndAcquireLock()) {
            return false;
          }
          return true;
        })) {
      m_cqService->receiveNotification(msg);
    }
  } catch (const CacheClosedException&) {
    LOGFINE("QueryService::receiveNotification: Cache has been closed.");
  }
}

std::shared_ptr<CacheableArrayList>
RemoteQueryService::getAllDurableCqsFromServer() const {
  return doIfNotDestroyed<std::shared_ptr<CacheableArrayList>>([&]() {
    // If cqService has not started, then no cq exists
    if (m_cqService) {
      return m_cqService->getAllDurableCqsFromServer();
    }

    return std::shared_ptr<CacheableArrayList>{};
  });
}

void RemoteQueryService::invokeCqConnectedListeners(ThinClientPoolDM* pool,
                                                    bool connected) {
  if (!m_cqService) {
    return;
  }

  std::string poolName;
  pool = dynamic_cast<ThinClientPoolDM*>(m_tccdm);
  if (pool != nullptr) {
    poolName = pool->getName();
    CqPoolsConnected::iterator itr = m_CqPoolsConnected.find(poolName);
    if (itr != m_CqPoolsConnected.end() && itr->second == connected) {
      LOGDEBUG("Returning since pools connection status matched.");
      return;
    } else {
      LOGDEBUG("Inserting since pools connection status did not match.");
      m_CqPoolsConnected[poolName] = connected;
    }
  }
  m_cqService->invokeCqConnectedListeners(poolName, connected);
}
void RemoteQueryService::initCqService() {
  if (m_cqService == nullptr) {
    LOGFINE("RemoteQueryService: starting cq service");
    m_cqService = std::make_shared<CqService>(m_tccdm, m_statisticsFactory);
    LOGFINE("RemoteQueryService: started cq service");
  }
}

}  // namespace client
}  // namespace geode
}  // namespace apache
