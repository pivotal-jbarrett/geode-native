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

#include "PdxTypeRegistry.hpp"

#include <boost/thread/lock_types.hpp>

#include <geode/PoolManager.hpp>

#include "CacheImpl.hpp"
#include "CacheRegionHelper.hpp"
#include "EnumInfo.hpp"
#include "PdxRemotePreservedData.hpp"
#include "ThinClientPoolDM.hpp"

namespace apache {
namespace geode {
namespace client {

PdxTypeRegistry::PdxTypeRegistry(CacheImpl* cache)
    : cache(cache),
      typeIdToPdxType(),
      remoteTypeIdToMergedPdxType(),
      localTypeToPdxType(),
      pdxTypeToTypeIdMap(),
      enumToInt(CacheableHashMap::create()),
      intToEnum(CacheableHashMap::create()) {}

PdxTypeRegistry::~PdxTypeRegistry() {}

size_t PdxTypeRegistry::testNumberOfPreservedData() const {
  return preserveData.size();
}

int32_t PdxTypeRegistry::getPDXIdForType(const std::string& type, Pool* pool,
                                         std::shared_ptr<PdxType> nType,
                                         bool checkIfThere) {
  if (checkIfThere) {
    auto lpdx = getLocalPdxType(type);
    if (lpdx != nullptr) {
      int id = lpdx->getTypeId();
      if (id != 0) {
        return id;
      }
    }
  }

  int typeId = cache->getSerializationRegistry()->GetPDXIdForType(pool, nType);
  nType->setTypeId(typeId);

  addPdxType(typeId, nType);
  return typeId;
}

int32_t PdxTypeRegistry::getPDXIdForType(std::shared_ptr<PdxType> nType,
                                         Pool* pool) {
  int32_t typeId = 0;
  {
    boost::shared_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
    auto&& iter = pdxTypeToTypeIdMap.find(nType);
    if (iter != pdxTypeToTypeIdMap.end()) {
      typeId = iter->second;
      if (typeId != 0) {
        return typeId;
      }
    }
  }

  {
    boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);

    auto&& iter = pdxTypeToTypeIdMap.find(nType);
    if (iter != pdxTypeToTypeIdMap.end()) {
      typeId = iter->second;
      if (typeId != 0) {
        return typeId;
      }
    }

    typeId = cache->getSerializationRegistry()->GetPDXIdForType(pool, nType);
    nType->setTypeId(typeId);
    pdxTypeToTypeIdMap.insert(std::make_pair(nType, typeId));
  }

  addPdxType(typeId, nType);
  return typeId;
}

void PdxTypeRegistry::clear() {
  {
    boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);

    typeIdToPdxType.clear();

    remoteTypeIdToMergedPdxType.clear();

    localTypeToPdxType.clear();

    if (intToEnum) intToEnum->clear();

    if (enumToInt) enumToInt->clear();

    pdxTypeToTypeIdMap.clear();
  }
  {
    boost::unique_lock<decltype(preservedDataMutex_)> lock(preservedDataMutex_);
    preserveData.clear();
  }
}

void PdxTypeRegistry::addPdxType(int32_t typeId,
                                 std::shared_ptr<PdxType> pdxType) {
  boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  std::pair<int32_t, std::shared_ptr<PdxType>> pc(typeId, pdxType);
  typeIdToPdxType.insert(pc);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getPdxType(int32_t typeId) const {
  boost::shared_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  auto&& iter = typeIdToPdxType.find(typeId);
  if (iter != typeIdToPdxType.end()) {
    return iter->second;
  }
  return nullptr;
}

void PdxTypeRegistry::addLocalPdxType(const std::string& localType,
                                      std::shared_ptr<PdxType> pdxType) {
  boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  localTypeToPdxType.emplace(localType, pdxType);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getLocalPdxType(
    const std::string& localType) const {
  boost::shared_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  auto&& it = localTypeToPdxType.find(localType);
  if (it != localTypeToPdxType.end()) {
    return it->second;
  }
  return nullptr;
}

void PdxTypeRegistry::setMergedType(int32_t remoteTypeId,
                                    std::shared_ptr<PdxType> mergedType) {
  boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  remoteTypeIdToMergedPdxType.emplace(remoteTypeId, mergedType);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getMergedType(
    int32_t remoteTypeId) const {
  auto&& it = remoteTypeIdToMergedPdxType.find(remoteTypeId);
  if (it != remoteTypeIdToMergedPdxType.end()) {
    return it->second;
  }
  return nullptr;
}

void PdxTypeRegistry::setPreserveData(
    std::shared_ptr<PdxSerializable> obj,
    std::shared_ptr<PdxRemotePreservedData> pData, TimerQueue& timerQueue) {
  boost::unique_lock<decltype(preservedDataMutex_)> lock(preservedDataMutex_);
  pData->setOwner(obj);
  if (preserveData.find(obj) != preserveData.end()) {
    // reset expiry task
    // TODO: check value for nullptr
    auto expTaskId = preserveData[obj]->getPreservedDataExpiryTaskId();
    expTaskId = timerQueue.reschedule(expTaskId, std::chrono::seconds(5));
    LOGDEBUG("PdxTypeRegistry::setPreserveData Reset expiry task Done");
    pData->setPreservedDataExpiryTaskId(expTaskId);
    preserveData[obj] = pData;
  } else {
    // schedule new expiry task
    auto id = timerQueue.schedule(std::chrono::seconds(20), [this, &obj]() {
      boost::unique_lock<decltype(preservedDataMutex_)> lock(
          preservedDataMutex_);
      LOGDEBUG("Entered PreservedDataExpiryHandler preserveData.size() = %d",
               preserveData.size());
      try {
        // remove the entry from the map
        preserveData.erase(obj);
      } catch (...) {
        // Ignore whatever exception comes
        LOGDEBUG(
            "PreservedDataExpiry:: Error while Clearing PdxObject and its "
            "preserved data. Ignoring the error");
      }
    });
    pData->setPreservedDataExpiryTaskId(id);
    LOGDEBUG(
        "PdxTypeRegistry::setPreserveData Schedule new timer task with id=%ld",
        id);
    preserveData.emplace(obj, pData);
  }

  LOGDEBUG(
      "PdxTypeRegistry::setPreserveData Successfully inserted new entry in "
      "preservedData");
}
std::shared_ptr<PdxRemotePreservedData> PdxTypeRegistry::getPreserveData(
    std::shared_ptr<PdxSerializable> pdxobj) const {
  boost::shared_lock<decltype(preservedDataMutex_)> lock(preservedDataMutex_);

  const auto& iter = preserveData.find((pdxobj));
  if (iter != preserveData.end()) {
    return iter->second;
  }
  return nullptr;
}

int32_t PdxTypeRegistry::getEnumValue(std::shared_ptr<EnumInfo> ei) {
  // TODO locking - naive concurrent optimization?
  std::shared_ptr<CacheableHashMap> tmp;
  tmp = enumToInt;
  const auto& entry = tmp->find(ei);
  if (entry != tmp->end()) {
    const auto val = std::dynamic_pointer_cast<CacheableInt32>(entry->second);
    return val->value();
  }

  boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  tmp = enumToInt;
  const auto& entry2 = tmp->find(ei);
  if (entry2 != tmp->end()) {
    const auto val2 = std::dynamic_pointer_cast<CacheableInt32>(entry2->second);
    return val2->value();
  }

  int val = static_cast<ThinClientPoolDM*>(
                cache->getPoolManager().getAll().begin()->second.get())
                ->GetEnumValue(ei);

  tmp = enumToInt;
  tmp->emplace(ei, CacheableInt32::create(val));
  enumToInt = tmp;
  return val;
}

std::shared_ptr<EnumInfo> PdxTypeRegistry::getEnum(int32_t enumVal) {
  // TODO locking - naive concurrent optimization?
  auto enumValPtr = CacheableInt32::create(enumVal);

  auto&& tmp = intToEnum;
  {
    auto&& entry = tmp->find(enumValPtr);
    if (entry != tmp->end()) {
      auto&& ret = std::dynamic_pointer_cast<EnumInfo>(entry->second);
      if (ret) {
        return std::move(ret);
      }
    }
  }

  boost::unique_lock<decltype(readerWriterMutex_)> lock(readerWriterMutex_);
  tmp = intToEnum;
  {
    auto&& entry = tmp->find(enumValPtr);
    if (entry != tmp->end()) {
      auto&& ret = std::dynamic_pointer_cast<EnumInfo>(entry->second);
      if (ret) {
        return std::move(ret);
      }
    }
  }

  auto&& ret = std::dynamic_pointer_cast<EnumInfo>(
      std::static_pointer_cast<ThinClientPoolDM>(
          cache->getPoolManager().getAll().begin()->second)
          ->GetEnum(enumVal));
  tmp = intToEnum;
  (*tmp)[enumValPtr] = ret;
  intToEnum = tmp;
  return std::move(ret);
}

}  // namespace client
}  // namespace geode
}  // namespace apache
