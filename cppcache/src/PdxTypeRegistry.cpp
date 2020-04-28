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

#include <geode/PoolManager.hpp>

#include "CacheImpl.hpp"
#include "CacheRegionHelper.hpp"
#include "ThinClientPoolDM.hpp"

namespace apache {
namespace geode {
namespace client {

PdxTypeRegistry::PdxTypeRegistry(CacheImpl* cache)
    : cache_(cache),
      typeIdToPdxType_(),
      remoteTypeIdToMergedPdxType_(),
      localTypeToPdxType_(),
      pdxTypeToTypeIdMap_(),
      enumToInt_(CacheableHashMap::create()),
      intToEnum_(CacheableHashMap::create()) {}

PdxTypeRegistry::~PdxTypeRegistry() {}

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

  int typeId = cache_->getSerializationRegistry()->GetPDXIdForType(pool, nType);
  nType->setTypeId(typeId);

  addPdxType(typeId, nType);
  return typeId;
}

int32_t PdxTypeRegistry::getPDXIdForType(std::shared_ptr<PdxType> pdxType,
                                         Pool* pool) {
  {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto&& iter = pdxTypeToTypeIdMap_.find(pdxType);
    if (iter != pdxTypeToTypeIdMap_.end()) {
      auto&& typeId = iter->second;
      if (typeId != 0) {
        return typeId;
      }
    }
  }

  {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);

    auto&& iter = pdxTypeToTypeIdMap_.find(pdxType);
    if (iter != pdxTypeToTypeIdMap_.end()) {
      auto&& typeId = iter->second;
      if (typeId != 0) {
        return typeId;
      }
    }

    auto&& typeId =
        cache_->getSerializationRegistry()->GetPDXIdForType(pool, pdxType);
    pdxType->setTypeId(typeId);
    pdxTypeToTypeIdMap_.emplace(pdxType, typeId);
    typeIdToPdxType_.emplace(typeId, pdxType);
    return typeId;
  }
}

void PdxTypeRegistry::clear() {
  {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    typeIdToPdxType_.clear();
    remoteTypeIdToMergedPdxType_.clear();
    localTypeToPdxType_.clear();
    if (intToEnum_) intToEnum_->clear();
    if (enumToInt_) enumToInt_->clear();
    pdxTypeToTypeIdMap_.clear();
  }
  {
    boost::unique_lock<boost::shared_mutex> lock(preserveDataMutex_);
    preserveData_.clear();
  }
}

void PdxTypeRegistry::addPdxType(int32_t typeId,
                                 std::shared_ptr<PdxType> pdxType) {
  boost::unique_lock<boost::shared_mutex> lock(mutex_);
  typeIdToPdxType_.emplace(typeId, pdxType);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getPdxType(int32_t typeId) const {
  boost::shared_lock<boost::shared_mutex> lock(mutex_);
  auto&& iter = typeIdToPdxType_.find(typeId);
  if (iter != typeIdToPdxType_.end()) {
    return iter->second;
  }
  return nullptr;
}

void PdxTypeRegistry::addLocalPdxType(const std::string& localType,
                                      std::shared_ptr<PdxType> pdxType) {
  boost::unique_lock<boost::shared_mutex> lock(mutex_);
  localTypeToPdxType_.emplace(localType, pdxType);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getLocalPdxType(
    const std::string& localType) const {
  boost::shared_lock<boost::shared_mutex> lock(mutex_);
  auto&& it = localTypeToPdxType_.find(localType);
  if (it != localTypeToPdxType_.end()) {
    return it->second;
  }
  return nullptr;
}

void PdxTypeRegistry::setMergedType(int32_t remoteTypeId,
                                    std::shared_ptr<PdxType> mergedType) {
  boost::unique_lock<boost::shared_mutex> lock(mutex_);
  remoteTypeIdToMergedPdxType_.emplace(remoteTypeId, mergedType);
}

std::shared_ptr<PdxType> PdxTypeRegistry::getMergedType(
    int32_t remoteTypeId) const {
  boost::shared_lock<boost::shared_mutex> lock(mutex_);
  auto&& it = remoteTypeIdToMergedPdxType_.find(remoteTypeId);
  if (it != remoteTypeIdToMergedPdxType_.end()) {
    return it->second;
  }
  return nullptr;
}

void PdxTypeRegistry::setPreserveData(
    std::shared_ptr<PdxSerializable> obj,
    std::shared_ptr<PdxRemotePreservedData> pData,
    ExpiryTaskManager& expiryTaskManager) {
  boost::unique_lock<boost::shared_mutex> lock(preserveDataMutex_);
  pData->setOwner(obj);
  auto&& entry = preserveData_.find(obj);
  if (entry != preserveData_.end()) {
    // reset expiry task
    auto&& expTaskId = entry->second->getPreservedDataExpiryTaskId();
    expiryTaskManager.resetTask(expTaskId, std::chrono::seconds(5));
    LOGDEBUG("PdxTypeRegistry::setPreserveData Reset expiry task Done");
    pData->setPreservedDataExpiryTaskId(expTaskId);
    entry->second = pData;
  } else {
    // schedule new expiry task
    auto handler = new PreservedDataExpiryHandler(shared_from_this(), obj);
    auto&& id = expiryTaskManager.scheduleExpiryTask(
        handler, std::chrono::seconds(20), std::chrono::seconds::zero(), false);
    pData->setPreservedDataExpiryTaskId(id);
    LOGDEBUG(
        "PdxTypeRegistry::setPreserveData Schedule new expiry task with id=%ld",
        id);
    preserveData_.emplace(obj, pData);
  }

  LOGDEBUG(
      "PdxTypeRegistry::setPreserveData Successfully inserted new entry in "
      "preservedData");
}
std::shared_ptr<PdxRemotePreservedData> PdxTypeRegistry::getPreserveData(
    const std::shared_ptr<PdxSerializable>& pdxobj) const {
  boost::shared_lock<boost::shared_mutex> lock(preserveDataMutex_);
  const auto& iter = preserveData_.find((pdxobj));
  if (iter != preserveData_.end()) {
    return iter->second;
  }
  return nullptr;
}

void PdxTypeRegistry::removePreserveData(
    const std::shared_ptr<PdxSerializable>& obj) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(preserveDataMutex_);
    LOGDEBUG("PdxTypeRegistry::removePreserveData: preserveData_.size() = %zu",
             preserveData_.size());
    preserveData_.erase(obj);
  } catch (...) {
    LOGDEBUG(
        "PdxTypeRegistry::removePreserveData: Error while Clearing PdxObject "
        "and its "
        "preserved data. Ignoring the error");
  }
}

size_t PdxTypeRegistry::testNumberOfPreservedData() const {
  boost::shared_lock<boost::shared_mutex> lock(preserveDataMutex_);
  return preserveData_.size();
}

int32_t PdxTypeRegistry::getEnumValue(std::shared_ptr<EnumInfo> ei) {
  {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto&& entry = enumToInt_->find(ei);
    if (entry != enumToInt_->end()) {
      return std::dynamic_pointer_cast<CacheableInt32>(entry->second)->value();
    }
  }

  {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto&& entry = enumToInt_->find(ei);
    if (entry != enumToInt_->end()) {
      return std::dynamic_pointer_cast<CacheableInt32>(entry->second)->value();
    }

    auto val = static_cast<ThinClientPoolDM*>(
                   cache_->getPoolManager().getAll().begin()->second.get())
                   ->GetEnumValue(ei);
    auto intValue = CacheableInt32::create(val);
    enumToInt_->emplace(ei, intValue);
    intToEnum_->emplace(intValue, ei);
    return val;
  }
}

std::shared_ptr<EnumInfo> PdxTypeRegistry::getEnum(int32_t enumVal) {
  auto enumValPtr = CacheableInt32::create(enumVal);

  {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto&& entry = intToEnum_->find(enumValPtr);
    if (entry != intToEnum_->end()) {
      if (auto&& ret = std::dynamic_pointer_cast<EnumInfo>(entry->second)) {
        return std::move(ret);
      }
    }
  }

  {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto&& entry = intToEnum_->find(enumValPtr);
    if (entry != intToEnum_->end()) {
      if (auto&& ret = std::dynamic_pointer_cast<EnumInfo>(entry->second)) {
        return std::move(ret);
      }
    }

    auto&& ret = std::dynamic_pointer_cast<EnumInfo>(
        std::static_pointer_cast<ThinClientPoolDM>(
            cache_->getPoolManager().getAll().begin()->second)
            ->GetEnum(enumVal));
    intToEnum_->emplace(enumValPtr, ret);
    enumToInt_->emplace(ret, enumValPtr);
    return std::move(ret);
  }
}
}  // namespace client
}  // namespace geode
}  // namespace apache
