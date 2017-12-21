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


#include <geode/RegionFactory.hpp>
#include <chrono>
#include <iosfwd>
#include <memory>
#include <string>

#include "CacheImpl.hpp"
#include <ace/OS_NS_stdio.h>
#include <geode/DiskPolicyType.hpp>
#include <geode/ExpirationAction.hpp>
#include <geode/RegionShortcut.hpp>
#include <geode/ExceptionTypes.hpp>
#include <geode/PoolManager.hpp>

namespace apache {
namespace geode {
namespace client {

extern ACE_Recursive_Thread_Mutex* g_disconnectLock;

class CacheListener;
class CacheLoader;
class CacheWriter;
class PartitionResolver;
class PersistenceManager;
class Properties;
class Region;
class RegionAttributes;

RegionFactory::RegionFactory(RegionShortcut preDefinedRegion,
                             CacheImpl* cacheImpl)
    : m_preDefinedRegion(preDefinedRegion),
      m_attributeFactory(std::make_shared<AttributesFactory>()),
      m_cacheImpl(cacheImpl) {
  setRegionShortcut();
}
std::shared_ptr<Region> RegionFactory::create(std::string name) {
  std::shared_ptr<Region> retRegionPtr = nullptr;
  std::shared_ptr<RegionAttributes> regAttr =
      m_attributeFactory->createRegionAttributes();
  if (m_preDefinedRegion != LOCAL && regAttr->getPoolName().empty()) {
    auto pool = m_cacheImpl->getPoolManager().getDefaultPool();
    if (!pool) {
      throw IllegalStateException("No pool for non-local region.");
    }
    m_attributeFactory->setPoolName(pool->getName());
    regAttr = m_attributeFactory->createRegionAttributes();
  }
  m_cacheImpl->createRegion(name, regAttr, retRegionPtr);

  return retRegionPtr;
}

void RegionFactory::setRegionShortcut() {
  switch (m_preDefinedRegion) {
    case PROXY: {
      m_attributeFactory->setCachingEnabled(false);
    } break;
    case CACHING_PROXY: {
      m_attributeFactory->setCachingEnabled(true);
    } break;
    case CACHING_PROXY_ENTRY_LRU: {
      m_attributeFactory->setCachingEnabled(true);
      m_attributeFactory->setLruEntriesLimit(DEFAULT_LRU_MAXIMUM_ENTRIES);
    } break;
    case LOCAL: {
    } break;
    case LOCAL_ENTRY_LRU: {
      m_attributeFactory->setLruEntriesLimit(DEFAULT_LRU_MAXIMUM_ENTRIES);
    } break;
  }
}

RegionFactory& RegionFactory::setCacheLoader(
    const std::shared_ptr<CacheLoader>& cacheLoader) {
  m_attributeFactory->setCacheLoader(cacheLoader);
  return *this;
}

RegionFactory& RegionFactory::setCacheWriter(
    const std::shared_ptr<CacheWriter>& cacheWriter) {
  m_attributeFactory->setCacheWriter(cacheWriter);
  return *this;
}
RegionFactory& RegionFactory::setCacheListener(
    const std::shared_ptr<CacheListener>& aListener) {
  m_attributeFactory->setCacheListener(aListener);
  return *this;
}
RegionFactory& RegionFactory::setPartitionResolver(
    const std::shared_ptr<PartitionResolver>& aResolver) {
  m_attributeFactory->setPartitionResolver(aResolver);
  return *this;
}

RegionFactory& RegionFactory::setCacheLoader(const std::string& lib,
                                             const std::string& func) {
  m_attributeFactory->setCacheLoader(lib, func);
  return *this;
}

RegionFactory& RegionFactory::setCacheWriter(const std::string& lib,
                                             const std::string& func) {
  m_attributeFactory->setCacheWriter(lib, func);
  return *this;
}

RegionFactory& RegionFactory::setCacheListener(const std::string& lib,
                                               const std::string& func) {
  m_attributeFactory->setCacheListener(lib, func);
  return *this;
}

RegionFactory& RegionFactory::setPartitionResolver(const std::string& lib,
                                                   const std::string& func) {
  m_attributeFactory->setPartitionResolver(lib, func);
  return *this;
}

RegionFactory& RegionFactory::setEntryIdleTimeout(
    ExpirationAction::Action action, std::chrono::seconds idleTimeout) {
  m_attributeFactory->setEntryIdleTimeout(action, idleTimeout);
  return *this;
}

RegionFactory& RegionFactory::setEntryTimeToLive(
    ExpirationAction::Action action, std::chrono::seconds timeToLive) {
  m_attributeFactory->setEntryTimeToLive(action, timeToLive);
  return *this;
}

RegionFactory& RegionFactory::setRegionIdleTimeout(
    ExpirationAction::Action action, std::chrono::seconds idleTimeout) {
  m_attributeFactory->setRegionIdleTimeout(action, idleTimeout);
  return *this;
}

RegionFactory& RegionFactory::setRegionTimeToLive(
    ExpirationAction::Action action, std::chrono::seconds timeToLive) {
  m_attributeFactory->setRegionTimeToLive(action, timeToLive);
  return *this;
}

RegionFactory& RegionFactory::setInitialCapacity(int initialCapacity) {
  char excpStr[256] = {0};
  if (initialCapacity < 0) {
    ACE_OS::snprintf(excpStr, 256, "initialCapacity must be >= 0 ");
    throw IllegalArgumentException(excpStr);
  }
  m_attributeFactory->setInitialCapacity(initialCapacity);
  return *this;
}

RegionFactory& RegionFactory::setLoadFactor(float loadFactor) {
  m_attributeFactory->setLoadFactor(loadFactor);
  return *this;
}

RegionFactory& RegionFactory::setConcurrencyLevel(uint8_t concurrencyLevel) {
  m_attributeFactory->setConcurrencyLevel(concurrencyLevel);
  return *this;
}
RegionFactory& RegionFactory::setConcurrencyChecksEnabled(bool enable) {
  m_attributeFactory->setConcurrencyChecksEnabled(enable);
  return *this;
}
RegionFactory& RegionFactory::setLruEntriesLimit(const uint32_t entriesLimit) {
  m_attributeFactory->setLruEntriesLimit(entriesLimit);
  return *this;
}

RegionFactory& RegionFactory::setDiskPolicy(
    const DiskPolicyType::PolicyType diskPolicy) {
  m_attributeFactory->setDiskPolicy(diskPolicy);
  return *this;
}

RegionFactory& RegionFactory::setCachingEnabled(bool cachingEnabled) {
  m_attributeFactory->setCachingEnabled(cachingEnabled);
  return *this;
}

RegionFactory& RegionFactory::setPersistenceManager(
    const std::shared_ptr<PersistenceManager>& persistenceManager,
    const std::shared_ptr<Properties>& config) {
  m_attributeFactory->setPersistenceManager(persistenceManager, config);
  return *this;
}

RegionFactory& RegionFactory::setPersistenceManager(
    const std::string& lib, const std::string& func,
    const std::shared_ptr<Properties>& config) {
  m_attributeFactory->setPersistenceManager(lib, func, config);
  return *this;
}

RegionFactory& RegionFactory::setPoolName(const std::string& name) {
  m_attributeFactory->setPoolName(name);
  return *this;
}

RegionFactory& RegionFactory::setCloningEnabled(bool isClonable) {
  m_attributeFactory->setCloningEnabled(isClonable);
  return *this;
}

}  // namespace client
}  // namespace geode
}  // namespace apache
