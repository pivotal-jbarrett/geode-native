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

#include "MemberListForVersionStamp.hpp"

#include <boost/thread/locks.hpp>

#include "util/Log.hpp"

namespace apache {
namespace geode {
namespace client {

MemberListForVersionStamp::MemberListForVersionStamp() { m_memberCounter = 0; }

MemberListForVersionStamp::~MemberListForVersionStamp() {}

/**
 * Add function searches whether the member is already added to the hash maps.
 */
uint16_t MemberListForVersionStamp::add(
    std::shared_ptr<DSMemberForVersionStamp> member) {
  boost::upgrade_lock<boost::shared_mutex> upgradeLock(mutex_);

  const auto& it = distributedMembers_.find(member->getHashKey());
  if (it != distributedMembers_.end()) {
    return it->second.id_;
  }

  boost::upgrade_to_unique_lock<boost::shared_mutex> lock(upgradeLock);
  const auto id = ++m_memberCounter;
  DistributedMemberWithIntIdentifier dmWithIntId(member, id);
  members_.emplace(id, member);
  distributedMembers_.emplace(member->getHashKey(), dmWithIntId);

  LOGDEBUG(
      "Adding a new member to the member list maintained for version stamps "
      "member Ids. HashKey: " +
      member->getHashKey() + " MemberCounter: " + std::to_string(id));

  return id;
}

std::shared_ptr<DSMemberForVersionStamp> MemberListForVersionStamp::getDSMember(
    uint16_t memberId) const {
  boost::shared_lock<boost::shared_mutex> lock(mutex_);

  const auto& it = members_.find(memberId);
  if (it != members_.end()) {
    return it->second;
  }

  return nullptr;
}

}  // namespace client
}  // namespace geode
}  // namespace apache
