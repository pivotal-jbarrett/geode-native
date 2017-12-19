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
#include <geode/ExpirationAttributes.hpp>

#include <geode/ExpirationAction.hpp>

using namespace apache::geode::client;

ExpirationAttributes::ExpirationAttributes()
    : m_action(ExpirationAction::INVALIDATE), m_timeout(0) {}

ExpirationAttributes::ExpirationAttributes(
    const std::chrono::seconds& expirationTime,
    const ExpirationAction::Action expirationAction)
    : m_action(ExpirationAction::INVALIDATE), m_timeout(expirationTime) {}
const std::chrono::seconds& ExpirationAttributes::getTimeout() const {
  return m_timeout;
}

void ExpirationAttributes::setTimeout(const std::chrono::seconds& timeout) {
  m_timeout = timeout;
}

ExpirationAction::Action ExpirationAttributes::getAction() const {
  return m_action;
}
void ExpirationAttributes::setAction(const ExpirationAction::Action& action) {
  m_action = action;
}
