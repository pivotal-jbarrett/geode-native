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

#include "PutAllPartialResult.hpp"

namespace apache {
namespace geode {
namespace client {

PutAllPartialResult::PutAllPartialResult(int totalMapSize,
                                         std::recursive_mutex& responseLock) {
  succeededKeys_ = std::make_shared<VersionedCacheableObjectPartList>(
      std::make_shared<std::vector<std::shared_ptr<CacheableKey>>>(),
      responseLock);
  totalMapSize_ = totalMapSize;
}

void PutAllPartialResult::consolidate(
    std::shared_ptr<PutAllPartialResult> other) {
  succeededKeys_->addAll(other->getSucceededKeysAndVersions());
  saveFailedKey(other->firstFailedKey_, other->firstCauseOfFailure_);
}

void PutAllPartialResult::addKeysAndVersions(
    std::shared_ptr<VersionedCacheableObjectPartList> keysAndVersion) {
  this->succeededKeys_->addAll(keysAndVersion);
}

void PutAllPartialResult::addKeys(
    std::shared_ptr<std::vector<std::shared_ptr<CacheableKey>>> m_keys) {
  if (succeededKeys_->getVersionedTagsize() > 0) {
    throw IllegalStateException(
        "attempt to store versionless keys when there are already versioned "
        "results");
  }
  this->succeededKeys_->addAllKeys(m_keys);
}

std::shared_ptr<VersionedCacheableObjectPartList>
PutAllPartialResult::getSucceededKeysAndVersions() {
  return succeededKeys_;
}

void PutAllPartialResult::saveFailedKey(std::shared_ptr<CacheableKey> key,
                                        std::shared_ptr<Exception> cause) {
  if (!key) {
    return;
  }

  if (!firstFailedKey_) {
    firstFailedKey_ = key;
    firstCauseOfFailure_ = cause;
  }
}

std::string PutAllPartialResult::toString() const {
  std::string toString =
      "PutAllPartialResult: [ Key = " + firstFailedKey_->toString() + " ]";

  if (totalMapSize_ > 0) {
    const auto size = succeededKeys_->size();
    const auto failedKeyNum = totalMapSize_ - size;
    if (failedKeyNum > 0) {
      toString += "The putAll operation failed to put " +
                  std::to_string(failedKeyNum) + " out of " +
                  std::to_string(totalMapSize_) + " entries ";
    } else {
      toString += "The putAll operation successfully put " +
                  std::to_string(size) + " out of " +
                  std::to_string(totalMapSize_) + " entries ";
    }
  }

  return toString;
}
}  // namespace client
}  // namespace geode
}  // namespace apache
