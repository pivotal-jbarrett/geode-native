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


#include <geode/DiskPolicyType.hpp>
#include <geode/SystemProperties.hpp>

#include "CacheImpl.hpp"
#include "EntriesMap.hpp"
#include "EntriesMapFactory.hpp"
#include "LRUAction.hpp"
#include "LRUExpMapEntry.hpp"
#include "LRUEntriesMap.hpp"

namespace apache {
namespace geode {
namespace client {
class RegionAttributes;
}  // namespace client
}  // namespace geode
}  // namespace apache

using namespace apache::geode::client;

/**
 * @brief Return a ConcurrentEntriesMap if no LRU, otherwise return a
 * LRUEntriesMap.
 * In the future, a EntriesMap facade can be put over the SharedRegionData to
 * support shared regions directly.
 */
EntriesMap* EntriesMapFactory::createMap(
    RegionInternal* region, const std::shared_ptr<RegionAttributes>& attrs) {
  EntriesMap* result = nullptr;
  uint32_t initialCapacity = attrs->getInitialCapacity();
  uint8_t concurrency = attrs->getConcurrencyLevel();
  /** @TODO will need a statistics entry factory... */
  uint32_t lruLimit = attrs->getLruEntriesLimit();
  const auto& ttl = attrs->getEntryTimeToLive();
  const auto& idle = attrs->getEntryIdleTimeout();
  bool concurrencyChecksEnabled = attrs->getConcurrencyChecksEnabled();
  bool heapLRUEnabled = false;

  auto cache = region->getCacheImpl();
  auto& prop = cache->getDistributedSystem().getSystemProperties();
  auto& expiryTaskmanager = cache->getExpiryTaskManager();

  if ((lruLimit != 0) || (prop.heapLRULimitEnabled())) {  // create LRU map...
    LRUAction::Action lruEvictionAction;
    DiskPolicyType::PolicyType dpType = attrs->getDiskPolicy();
    if (dpType == DiskPolicyType::OVERFLOWS) {
      lruEvictionAction = LRUAction::OVERFLOW_TO_DISK;
    } else if ((dpType == DiskPolicyType::NONE) ||
               (prop.heapLRULimitEnabled())) {
      lruEvictionAction = LRUAction::LOCAL_DESTROY;
      if (prop.heapLRULimitEnabled()) heapLRUEnabled = true;
    } else {
      return nullptr;
    }
    if (ttl.count() != 0 || idle.count() != 0) {
      result = new LRUEntriesMap(
          &expiryTaskmanager,
          std::unique_ptr<LRUExpEntryFactory>(
              new LRUExpEntryFactory(concurrencyChecksEnabled)),
          region, lruEvictionAction, lruLimit, concurrencyChecksEnabled,
          concurrency, heapLRUEnabled);
    } else {
      result = new LRUEntriesMap(
          &expiryTaskmanager,
          std::unique_ptr<LRUEntryFactory>(
              new LRUEntryFactory(concurrencyChecksEnabled)),
          region, lruEvictionAction, lruLimit, concurrencyChecksEnabled,
          concurrency, heapLRUEnabled);
    }
  } else if (ttl.count() > 0 || idle.count() > 0) {
    // create entries with a ExpEntryFactory.
    result = new ConcurrentEntriesMap(
        &expiryTaskmanager,
        std::unique_ptr<ExpEntryFactory>(
            new ExpEntryFactory(concurrencyChecksEnabled)),
        concurrencyChecksEnabled, region, concurrency);
  } else {
    // create plain concurrent map.
    result = new ConcurrentEntriesMap(
        &expiryTaskmanager,
        std::unique_ptr<EntryFactory>(
            new EntryFactory(concurrencyChecksEnabled)),
        concurrencyChecksEnabled, region, concurrency);
  }
  result->open(initialCapacity);
  return result;
}
