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

#include "CqEvent.hpp"
#include "Log.hpp"
#include "impl/SafeConvert.hpp"
#include "CacheableBuiltins.hpp"
using namespace System;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

GENERIC(class TKey, class TResult)
gc_ptr(CqQuery<TKey, TResult>) CqEvent<TKey, TResult>::getCq() {
  std::shared_ptr<native::CqQuery>& cQueryptr(m_nativeptr->getCq());
  return CqQuery<TKey, TResult>::Create(cQueryptr);
}

GENERIC(class TKey, class TResult)
CqOperation CqEvent<TKey, TResult>::getBaseOperation() { return CqOperation(m_nativeptr->getBaseOperation()); }

GENERIC(class TKey, class TResult)
CqOperation CqEvent<TKey, TResult>::getQueryOperation() { return CqOperation(m_nativeptr->getQueryOperation()); }

GENERIC(class TKey, class TResult)
TKey CqEvent<TKey, TResult>::getKey() {
  std::shared_ptr<native::CacheableKey>& keyptr(m_nativeptr->getKey());
  return TypeRegistry::GetManagedValueGeneric<TKey>(keyptr);
}

GENERIC(class TKey, class TResult)
TResult CqEvent<TKey, TResult>::getNewValue() {
  std::shared_ptr<native::Cacheable>& valptr(m_nativeptr->getNewValue());
  return TypeRegistry::GetManagedValueGeneric<TResult>(valptr);
}

GENERIC(class TKey, class TResult)
gc_ptr(array<Byte>) CqEvent<TKey, TResult>::getDeltaValue() {
  auto deltaBytes = m_nativeptr->getDeltaValue();
  auto managedDeltaBytes = (gc_ptr(CacheableBytes))CacheableBytes::Create(deltaBytes);
  return (gc_ptr(array<Byte>))managedDeltaBytes;
}
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
