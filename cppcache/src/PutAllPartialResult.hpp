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

#ifndef GEODE_PUTALLPARTIALRESULT_H_
#define GEODE_PUTALLPARTIALRESULT_H_

#include <geode/CacheableString.hpp>
#include <geode/Serializable.hpp>

#include "VersionedCacheableObjectPartList.hpp"

namespace apache {
namespace geode {
namespace client {

class PutAllPartialResult final : public Serializable {
 private:
  std::shared_ptr<VersionedCacheableObjectPartList> succeededKeys_;
  std::shared_ptr<CacheableKey> firstFailedKey_;
  std::shared_ptr<Exception> firstCauseOfFailure_;
  int32_t totalMapSize_;

 public:
  PutAllPartialResult(int totalMapSize, std::recursive_mutex& responseLock);
  ~PutAllPartialResult() noexcept override {}

  /**
   * Add all succeededKeys and firstfailedKey.
   * Before calling this, we must read PutAllPartialResultServerException and
   * formulate obj of type PutAllPartialResult.
   */
  void consolidate(std::shared_ptr<PutAllPartialResult> other);

  std::shared_ptr<Exception> getFailure() { return firstCauseOfFailure_; }

  void addKeysAndVersions(
      std::shared_ptr<VersionedCacheableObjectPartList> keysAndVersion);

  void addKeys(
      std::shared_ptr<std::vector<std::shared_ptr<CacheableKey>>> m_keys);

  void saveFailedKey(std::shared_ptr<CacheableKey> key,
                     std::shared_ptr<Exception> cause);

  std::shared_ptr<VersionedCacheableObjectPartList>
  getSucceededKeysAndVersions();

  // Returns the first key that failed
  std::shared_ptr<CacheableKey> getFirstFailedKey() { return firstFailedKey_; }

  // Returns there's failedKeys
  bool hasFailure() { return firstFailedKey_ != nullptr; }

  std::string toString() const override;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PUTALLPARTIALRESULT_H_
