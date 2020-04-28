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

#pragma once

#ifndef GEODE_REMOTEQUERYSERVICE_H_
#define GEODE_REMOTEQUERYSERVICE_H_

#include <memory>
#include <string>

#include <geode/QueryService.hpp>
#include <geode/internal/geode_globals.hpp>

#include "CqService.hpp"
#include "ThinClientCacheDistributionManager.hpp"
#include "statistics/StatisticsManager.hpp"
#include "util/shared_mutex.hpp"

namespace apache {
namespace geode {
namespace client {

class CacheImpl;
class ThinClientPoolDM;
typedef std::map<std::string, bool> CqPoolsConnected;
class APACHE_GEODE_EXPORT RemoteQueryService
    : public QueryService,
      public std::enable_shared_from_this<RemoteQueryService> {
 public:
  explicit RemoteQueryService(CacheImpl* cptr,
                              ThinClientPoolDM* poolDM = nullptr);
  virtual ~RemoteQueryService() = default;

  void init();

  inline const volatile bool& invalid() { return m_invalid; }

  void close();

  std::shared_ptr<Query> newQuery(std::string querystring) override;

  virtual std::shared_ptr<CqQuery> newCq(
      std::string querystr, const std::shared_ptr<CqAttributes>& cqAttr,
      bool isDurable = false) override;

  virtual std::shared_ptr<CqQuery> newCq(
      std::string name, std::string querystr,
      const std::shared_ptr<CqAttributes>& cqAttr,
      bool isDurable = false) override;

  virtual void closeCqs() override;

  virtual QueryService::query_container_type getCqs() const override;

  virtual std::shared_ptr<CqQuery> getCq(
      const std::string& name) const override;

  virtual void executeCqs() override;

  virtual void stopCqs() override;

  virtual std::shared_ptr<CqServiceStatistics> getCqServiceStatistics()
      const override;

  void executeAllCqs(bool failover);

  virtual std::shared_ptr<CacheableArrayList> getAllDurableCqsFromServer()
      const override;

  /**
   * execute all cqs on the endpoint after failover
   */
  GfErrType executeAllCqs(TcrEndpoint* endpoint);
  void receiveNotification(TcrMessage* msg);
  void invokeCqConnectedListeners(ThinClientPoolDM* pool, bool connected);

  template <typename _Return, typename _Function>
  inline _Return doIfNotDestroyed(_Function function) const {
    boost::shared_lock<boost::shared_mutex> lock(mutex_, boost::defer_lock);
    while (!lock.try_lock()) {
      if (m_invalid) {
        break;
      }
    }

    if (m_invalid) {
      throw CacheClosedException("QueryService: Cache has been closed.");
    }

    return function();
  }

 private:
  volatile bool m_invalid;
  mutable boost::shared_mutex mutex_;

  ThinClientBaseDM* m_tccdm;
  std::shared_ptr<CqService> m_cqService;
  CqPoolsConnected m_CqPoolsConnected;
  statistics::StatisticsFactory* m_statisticsFactory;

  /**
   *   For Lazy Cq Start-no use, no start
   */
  void initCqService();
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_REMOTEQUERYSERVICE_H_
