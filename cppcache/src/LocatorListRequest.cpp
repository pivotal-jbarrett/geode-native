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
#include "GeodeTypeIdsImpl.hpp"
#include "LocatorListRequest.hpp"
#include <geode/DataOutput.hpp>

using namespace apache::geode::client;

LocatorListRequest::LocatorListRequest(const std::string& servergroup)
    : m_servergroup(servergroup) {}

void LocatorListRequest::toData(DataOutput& output) const {
  output.writeNativeString(m_servergroup.c_str());
}
void LocatorListRequest::fromData(DataInput& input) {}

int8_t LocatorListRequest::typeId() const {
  return static_cast<int8_t>(GeodeTypeIdsImpl::LocatorListRequest);
}

size_t LocatorListRequest::objectSize() const { return 0; }
