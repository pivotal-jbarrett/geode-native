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

#include "CacheableKey.hpp"
#include "CacheableString.hpp"
#include "CacheableBuiltins.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace native = apache::geode::client;

System::Int32 CacheableKey::GetHashCode() {
  try {
    return dynamic_cast<native::CacheableKey*>(m_nativeptr->get())->hashcode();
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

bool CacheableKey::Equals(gc_ptr(Client::ICacheableKey) other) {
  try {
    return dynamic_cast<native::CacheableKey*>(m_nativeptr->get())
        ->
        operator==(*dynamic_cast<native::CacheableKey*>(((gc_ptr(Client::CacheableKey))other)->m_nativeptr->get()));
  } finally {
    GC::KeepAlive(m_nativeptr);
    GC::KeepAlive(((gc_ptr(Client::CacheableKey))other)->m_nativeptr);
  }
}

bool CacheableKey::Equals(gc_ptr(Object) obj) { return Equals(dynamic_cast<gc_ptr(CacheableKey)>(obj)); }

CacheableKey::operator gc_ptr(CacheableKey)(Byte value) {
  return (gc_ptr(CacheableKey))CacheableByte::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(bool value) {
  return (gc_ptr(CacheableKey))CacheableBoolean::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(Char value) {
  return (gc_ptr(CacheableKey))CacheableCharacter::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(Double value) {
  return (gc_ptr(CacheableKey))CacheableDouble::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(Single value) {
  return (gc_ptr(CacheableKey))CacheableFloat::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(System::Int16 value) {
  return (gc_ptr(CacheableKey))CacheableInt16::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(System::Int32 value) {
  return (gc_ptr(CacheableKey))CacheableInt32::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(System::Int64 value) {
  return (gc_ptr(CacheableKey))CacheableInt64::Create(value);
}

CacheableKey::operator gc_ptr(CacheableKey)(gc_ptr(String) value) {
  return dynamic_cast<gc_ptr(CacheableKey)>(CacheableString::Create(value));
}
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
