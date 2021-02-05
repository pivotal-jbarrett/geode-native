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

#include "begin_native.hpp"
#include <tuple>
#include "geode/Region.hpp"
#include "geode/Cache.hpp"
#include "end_native.hpp"

#include "Region.hpp"
#include "Cache.hpp"
#include "CacheStatistics.hpp"
#include "RegionAttributes.hpp"
#include "AttributesMutator.hpp"
#include "RegionEntry.hpp"
#include "ISelectResults.hpp"
#include "ISerializable.hpp"
#include "ResultSet.hpp"
#include "StructSet.hpp"
#include "impl/AuthenticatedView.hpp"
#include "impl/SafeConvert.hpp"
#include "LocalRegion.hpp"
#include "Pool.hpp"
#include "PoolManager.hpp"
#include "SystemProperties.hpp"
#include "impl/CacheResolver.hpp"
#include "TimeUtils.hpp"

namespace Apache {
namespace Geode {
namespace Client {

using namespace System;
using namespace msclr::interop;

namespace native = apache::geode::client;

GENERIC(class TKey, class TValue)
TValue Region<TKey, TValue>::Get(TKey key, gc_ptr(Object) callbackArg) {
  std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
  std::shared_ptr<native::Serializable> callbackptr =
      Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
  GC::KeepAlive(m_nativeptr);
  auto nativeptr = this->get(keyptr, callbackptr);
  if (nativeptr == nullptr) {
    throw gcnew KeyNotFoundException("The given key was not present in the region.");
  }
  TValue returnVal = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return returnVal;
}

GENERIC(class TKey, class TValue)
TValue Region<TKey, TValue>::Get(TKey key) { return Get(key, nullptr); }

GENERIC(class TKey, class TValue)
std::shared_ptr<native::Serializable> Region<TKey, TValue>::get(std::shared_ptr<native::CacheableKey>& keyptr,
                                                                std::shared_ptr<native::Serializable>& callbackptr) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      return m_nativeptr->get()->get(keyptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
std::shared_ptr<native::Serializable> Region<TKey, TValue>::get(std::shared_ptr<native::CacheableKey>& keyptr) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      return m_nativeptr->get()->get(keyptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::isPoolInMultiuserMode() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    auto rAttributes = this->Attributes;
    auto poolName = rAttributes->PoolName;
    if (poolName != nullptr) {
      auto poolManager = gcnew PoolManager(m_nativeptr->get()->getCache().getPoolManager());
      auto pool = poolManager->Find(poolName);
      if (pool != nullptr && !pool->Destroyed) {
        return pool->MultiuserAuthentication;
      }
    }
    return false;
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Put(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->put(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Put(TKey key, TValue value) { Put(key, value, nullptr); }

GENERIC(class TKey, class TValue)
TValue Region<TKey, TValue>::default ::get(TKey key) {
  std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
  GC::KeepAlive(m_nativeptr);
  auto nativeptr = this->get(keyptr);
  if (nativeptr == nullptr) {
    throw gcnew KeyNotFoundException("The given key was not present in the region.");
  }
  TValue returnVal = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return returnVal;
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::default ::set(TKey key, TValue value) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      m_nativeptr->get()->put(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
System::Collections::Generic::IEnumerator<KeyValuePair<TKey, TValue>> ^ Region<TKey, TValue>::GetEnumerator() {
  std::vector<std::shared_ptr<native::RegionEntry>> vc;

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  auto toArray = gcnew array<KeyValuePair<TKey, TValue>>(static_cast<int>(vc.size()));

  for (System::Int32 index = 0; index < toArray->Length; index++) {
    auto& nativeptr = vc[index];
    auto key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
    auto val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
    toArray[index] = KeyValuePair<TKey, TValue>(key, val);
  }
  return ((System::Collections::Generic::IEnumerable<KeyValuePair<TKey, TValue>> ^) toArray)->GetEnumerator();
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::IEnumerator) Region<TKey, TValue>::GetEnumeratorOld() {
  std::vector<std::shared_ptr<native::RegionEntry>> vc;

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  auto toArray = gcnew array<gc_ptr(Object)>(static_cast<int>(vc.size()));

  for (System::Int32 index = 0; index < toArray->Length; index++) {
    auto& nativeptr = vc[index];
    auto key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
    auto val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
    toArray[index] = KeyValuePair<TKey, TValue>(key, val);
  }
  return ((gc_ptr(System::Collections::Generic::IEnumerable<Object ^>)) toArray)->GetEnumerator();
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::Contains(KeyValuePair<TKey, TValue> keyValuePair) {
  std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
  GC::KeepAlive(m_nativeptr);
  auto nativeptr = this->get(keyptr);
  // This means that key is not present.
  if (nativeptr == nullptr) {
    return false;
  }
  auto value = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return ((gc_ptr(Object))value)->Equals(keyValuePair.Value);
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::ContainsKey(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      return m_nativeptr->get()->containsKeyOnServer(keyptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::TryGetValue(TKey key, TValue % val) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
    GC::KeepAlive(m_nativeptr);
    auto nativeptr = this->get(keyptr);
    if (nativeptr == nullptr) {
      val = TValue();
      return false;
    } else {
      val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
      return true;
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<TKey>) Region<TKey, TValue>::Keys::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::CacheableKey>> vc;
    try {
      vc = m_nativeptr->get()->serverKeys();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto keyarr = gcnew array<TKey>(static_cast<int>(vc.size()));
    for (System::Int32 index = 0; index < keyarr->Length; index++) {
      auto& nativeptr = vc[index];
      keyarr[index] = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr);
    }
    auto collectionlist = (gc_ptr(System::Collections::Generic::ICollection<TKey>))keyarr;
    return collectionlist;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<TValue>) Region<TKey, TValue>::Values::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::Cacheable>> vc;
    try {
      vc = m_nativeptr->get()->values();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto valarr = gcnew array<TValue>(static_cast<int>(vc.size()));
    for (System::Int32 index = 0; index < vc.size(); index++) {
      auto& nativeptr = vc[index];
      valarr[index] = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
    }
    auto collectionlist = (gc_ptr(System::Collections::Generic::ICollection<TValue>))valarr;
    return collectionlist;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Add(TKey key, TValue value) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      m_nativeptr->get()->create(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Add(KeyValuePair<TKey, TValue> keyValuePair) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValuePair.Value);
      m_nativeptr->get()->create(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Add(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->create(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::Remove(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      return m_nativeptr->get()->removeEx(keyptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::Remove(TKey key, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      return m_nativeptr->get()->removeEx(keyptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::Remove(KeyValuePair<TKey, TValue> keyValuePair) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValuePair.Value);
      return m_nativeptr->get()->remove(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::Remove(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      GC::KeepAlive(m_nativeptr);
      return m_nativeptr->get()->remove(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::InvalidateRegion() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    InvalidateRegion(nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::InvalidateRegion(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->invalidateRegion(callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::DestroyRegion() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    DestroyRegion(nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::DestroyRegion(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->destroyRegion(callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Invalidate(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    Invalidate(key, nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Invalidate(TKey key, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->invalidate(keyptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    return PutAll(map, TimeUtils::DurationToTimeSpan(native::DEFAULT_RESPONSE_TIMEOUT));

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map,
                                  TimeSpan timeout) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    native::HashMapOfCacheable nativeMap;
    FOR_EACH (KeyValuePair<TKey, TValue> keyValPair in map) {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValPair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValPair.Value);
      nativeMap.emplace(keyptr, valueptr);
    }
    try {
      m_nativeptr->get()->putAll(nativeMap, TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeout));
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map, TimeSpan timeout,
                                  gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    native::HashMapOfCacheable nativeMap;
    FOR_EACH (KeyValuePair<TKey, TValue> keyValPair in map) {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValPair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValPair.Value);
      nativeMap.emplace(keyptr, valueptr);
    }
    std::shared_ptr<native::Serializable> callbackptr =
        Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
    try {
      m_nativeptr->get()->putAll(nativeMap, TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeout),
                                 callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, System::Exception ^>)
                                      exceptions) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    GetAll(keys, values, exceptions, false);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, System::Exception ^>)
                                      exceptions,
                                  bool addToLocalCache) {
  if (keys != nullptr) {
    _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

      std::vector<std::shared_ptr<native::CacheableKey>> vecKeys;

      FOR_EACH (TKey item in keys) {
        auto v = Serializable::GetUnmanagedValueGeneric<TKey>(item);
        vecKeys.push_back(v);
      }

      native::HashMapOfCacheable native_value;

      try {
        native_value = m_nativeptr->get()->getAll(vecKeys);
      } finally {
        GC::KeepAlive(m_nativeptr);
      }
      if (values != nullptr) {
        for (const auto& iter : native_value) {
          TKey key = TypeRegistry::GetManagedValueGeneric<TKey>(iter.first);
          TValue val = TypeRegistry::GetManagedValueGeneric<TValue>(iter.second);
          values->Add(key, val);
        }
      }

    _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
  } else {
    throw gcnew IllegalArgumentException("GetAll: null keys provided");
  }
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                                  gc_ptr(System::Collections::Generic::IDictionary<TKey, System::Exception ^>)
                                      exceptions,
                                  bool addToLocalCache, gc_ptr(Object) callbackArg) {
  if (keys != nullptr) {
    _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

      std::vector<std::shared_ptr<native::CacheableKey>> vecKeys;

      FOR_EACH (TKey item in keys) {
        auto v = Serializable::GetUnmanagedValueGeneric<TKey>(item);
        vecKeys.push_back(v);
      }

      std::shared_ptr<native::HashMapOfCacheable> valuesPtr;
      if (values != nullptr) {
        valuesPtr = std::make_shared<native::HashMapOfCacheable>();
      }
      auto callbackptr = Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      native::HashMapOfCacheable native_value;
      try {
        native_value = m_nativeptr->get()->getAll(vecKeys, callbackptr);
      } finally {
        GC::KeepAlive(m_nativeptr);
      }
      if (values != nullptr) {
        for (const auto& iter : native_value) {
          TKey key = TypeRegistry::GetManagedValueGeneric<TKey>(iter.first);
          TValue val = TypeRegistry::GetManagedValueGeneric<TValue>(iter.second);
          values->Add(key, val);
        }
      }

    _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
  } else {
    throw gcnew IllegalArgumentException("GetAll: null keys provided");
  }
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    RemoveAll(keys, nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                                     gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::CacheableKey>> vecKeys;
    FOR_EACH (TKey item in keys) {
      auto v = Serializable::GetUnmanagedValueGeneric<TKey>(item);
      vecKeys.push_back(v);
    }

    std::shared_ptr<native::Serializable> callbackptr =
        Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);

    try {
      m_nativeptr->get()->removeAll(vecKeys, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}
GENERIC(class TKey, class TValue)
gc_ptr(String) Region<TKey, TValue>::Name::get() {
  try {
    return marshal_as<gc_ptr(String)>(m_nativeptr->get()->getName());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
gc_ptr(String) Region<TKey, TValue>::FullPath::get() {
  try {
    return marshal_as<gc_ptr(String)>(m_nativeptr->get()->getFullPath());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) Region<TKey, TValue>::ParentRegion::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto parentRegion = m_nativeptr->get()->getParentRegion();
      return Region::Create(parentRegion);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) Region<TKey, TValue>::Attributes::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeptr = m_nativeptr->get()->getAttributes();
      return Apache::Geode::Client::RegionAttributes<TKey, TValue>::Create(nativeptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(AttributesMutator<TKey, TValue>) Region<TKey, TValue>::AttributesMutator::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto am = m_nativeptr->get()->getAttributesMutator();
      return Apache::Geode::Client::AttributesMutator<TKey, TValue>::Create(am);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(Apache::Geode::Client::CacheStatistics) Region<TKey, TValue>::Statistics::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeptr = m_nativeptr->get()->getStatistics();
      return Apache::Geode::Client::CacheStatistics::Create(nativeptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) Region<TKey, TValue>::GetSubRegion(gc_ptr(String) path) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto subRegion = m_nativeptr->get()->getSubregion(marshal_as<std::string>(path));
      return Region::Create(subRegion);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) Region<TKey, TValue>::CreateSubRegion(
    gc_ptr(String) subRegionName, gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) attributes) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto p_attrs = attributes->GetNative();
      return Region::Create(m_nativeptr->get()->createSubregion(marshal_as<std::string>(subRegionName), *p_attrs));
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
System::Collections::Generic::ICollection<gc_ptr(IRegion<TKey, TValue>)> ^
    Region<TKey, TValue>::SubRegions(bool recursive) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::Region>> vsr;
    try {
      vsr = m_nativeptr->get()->subregions(recursive);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto subRegions = gcnew array<gc_ptr(IRegion<TKey, TValue>)>(static_cast<int>(vsr.size()));

    for (System::Int32 index = 0; index < subRegions->Length; index++) {
      auto subRegion = vsr[index];
      subRegions[index] = Region<TKey, TValue>::Create(subRegion);
    }
    auto collection = (System::Collections::Generic::ICollection<gc_ptr(IRegion<TKey, TValue>)> ^) subRegions;
    return collection;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionEntry<TKey, TValue>) Region<TKey, TValue>::GetEntry(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      auto nativeptr = m_nativeptr->get()->getEntry(keyptr);
      return RegionEntry<TKey, TValue>::Create(nativeptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
System::Collections::Generic::ICollection<gc_ptr(RegionEntry<TKey, TValue>)> ^
    Region<TKey, TValue>::GetEntries(bool recursive) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::RegionEntry>> vc;
    try {
      vc = m_nativeptr->get()->entries(recursive);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto entryarr = gcnew array<gc_ptr(RegionEntry<TKey, TValue>)>(static_cast<int>(vc.size()));

    for (System::Int32 index = 0; index < entryarr->Length; index++) {
      auto& nativeptr = vc[index];
      entryarr[index] = RegionEntry<TKey, TValue>::Create(nativeptr);
    }
    auto collection = (System::Collections::Generic::ICollection<gc_ptr(RegionEntry<TKey, TValue>)> ^) entryarr;

    return collection;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegionService) Region<TKey, TValue>::RegionService::get() {
  return CacheResolver::Lookup(&m_nativeptr->get()->getCache());
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::ContainsValueForKey(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      return m_nativeptr->get()->containsValueForKey(keyptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
int Region<TKey, TValue>::Count::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      return m_nativeptr->get()->size();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Clear() { Clear(nullptr); }

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::Clear(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->clear(callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::CopyTo(array<KeyValuePair<TKey, TValue>> ^ toArray, int startIdx) {
  if (toArray == nullptr) {
    throw gcnew System::ArgumentNullException;
  }
  if (startIdx < 0) {
    throw gcnew System::ArgumentOutOfRangeException;
  }

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::RegionEntry>> vc;
    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

    if (toArray->Rank > 1 || (vc.size() > (toArray->Length - startIdx))) {
      throw gcnew System::ArgumentException;
    }

    for (System::Int32 index = 0; index < vc.size(); index++) {
      auto& nativeptr = vc[index];
      auto key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
      auto val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
      toArray[startIdx] = KeyValuePair<TKey, TValue>(key, val);
      ++startIdx;
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::IsDestroyed::get() {
  try {
    return m_nativeptr->get()->isDestroyed();
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterKeys(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys) {
  RegisterKeys(keys, false, false);
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterKeys(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys, bool isDurable,
                                        bool getInitialValues) {
  RegisterKeys(keys, isDurable, getInitialValues, true);
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterKeys(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys, bool isDurable,
                                        bool getInitialValues, bool receiveValues) {
  if (keys != nullptr) {
    _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

      std::vector<std::shared_ptr<native::CacheableKey>> vecKeys;

      FOR_EACH (TKey item in keys) {
        auto v = Serializable::GetUnmanagedValueGeneric<TKey>(item);
        vecKeys.push_back(v);
      }
      try {
        m_nativeptr->get()->registerKeys(vecKeys, isDurable, getInitialValues, receiveValues);
      } finally {
        GC::KeepAlive(m_nativeptr);
      }

    _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
  }
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::UnregisterKeys(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys) {
  if (keys != nullptr) {
    _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

      std::vector<std::shared_ptr<native::CacheableKey>> vecKeys;

      FOR_EACH (TKey item in keys) {
        auto v = Serializable::GetUnmanagedValueGeneric<TKey>(item);
        vecKeys.push_back(v);
      }

      try {
        m_nativeptr->get()->unregisterKeys(vecKeys);
      } finally {
        GC::KeepAlive(m_nativeptr);
      }

    _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
  }
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterAllKeys() { RegisterAllKeys(false, false); }

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterAllKeys(bool isDurable) { RegisterAllKeys(isDurable, false); }

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterAllKeys(bool isDurable, bool getInitialValues) {
  RegisterAllKeys(isDurable, getInitialValues, true);
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterAllKeys(bool isDurable, bool getInitialValues, bool receiveValues) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->registerAllKeys(isDurable, getInitialValues, receiveValues);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<TKey>) Region<TKey, TValue>::GetInterestList() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::CacheableKey>> vc;
    try {
      vc = m_nativeptr->get()->getInterestList();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto keyarr = gcnew array<TKey>(static_cast<int>(vc.size()));
    for (System::Int32 index = 0; index < keyarr->Length; index++) {
      auto& nativeptr = vc[index];
      keyarr[index] = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr);
    }

    auto collectionlist = (gc_ptr(System::Collections::Generic::ICollection<TKey>))keyarr;
    return collectionlist;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<String ^>) Region<TKey, TValue>::GetInterestListRegex() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<native::CacheableString>> vc;
    try {
      vc = m_nativeptr->get()->getInterestListRegex();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto strarr = gcnew array<gc_ptr(String)>(static_cast<int>(vc.size()));
    for (System::Int32 index = 0; index < strarr->Length; index++) {
      strarr[index] = marshal_as<gc_ptr(String)>(vc[index]->value());
      // collectionlist[ index ] = Serializable::GetManagedValue<TValue>(nativeptr);
    }
    auto collectionlist = (gc_ptr(System::Collections::Generic::ICollection<String ^>)) strarr;
    return collectionlist;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::UnregisterAllKeys() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->unregisterAllKeys();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterRegex(gc_ptr(String) regex) { RegisterRegex(regex, false, false); }

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterRegex(gc_ptr(String) regex, bool isDurable) {
  RegisterRegex(regex, isDurable, false);
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterRegex(gc_ptr(String) regex, bool isDurable, bool getInitialValues) {
  RegisterRegex(regex, isDurable, getInitialValues, true);
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::RegisterRegex(gc_ptr(String) regex, bool isDurable, bool getInitialValues,
                                         bool receiveValues) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->registerRegex(marshal_as<std::string>(regex), isDurable, getInitialValues, receiveValues);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void Region<TKey, TValue>::UnregisterRegex(gc_ptr(String) regex) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->unregisterRegex(marshal_as<std::string>(regex));
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
GENERIC(class TResult)
gc_ptr(ISelectResults<TResult>) Region<TKey, TValue>::Query(gc_ptr(String) predicate) {
  return Query<TResult>(predicate, TimeUtils::DurationToTimeSpan(native::DEFAULT_QUERY_RESPONSE_TIMEOUT));
}

GENERIC(class TKey, class TValue)
GENERIC(class TResult)
gc_ptr(ISelectResults<TResult>) Region<TKey, TValue>::Query(gc_ptr(String) predicate, TimeSpan timeout) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto selectResults = m_nativeptr->get()->query(marshal_as<std::string>(predicate),
                                                     TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeout));
      if (auto resultptr = std::dynamic_pointer_cast<native::ResultSet>(selectResults)) {
        return ResultSet<TResult>::Create(resultptr);
      } else if (auto structptr = std::dynamic_pointer_cast<native::StructSet>(selectResults)) {
        return StructSet<TResult>::Create(structptr);
      }
      return nullptr;
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::ExistsValue(gc_ptr(String) predicate) {
  return ExistsValue(predicate, TimeUtils::DurationToTimeSpan(native::DEFAULT_QUERY_RESPONSE_TIMEOUT));
}

GENERIC(class TKey, class TValue)
bool Region<TKey, TValue>::ExistsValue(gc_ptr(String) predicate, TimeSpan timeout) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      return m_nativeptr->get()->existsValue(marshal_as<std::string>(predicate),
                                             TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeout));
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(Object) Region<TKey, TValue>::SelectValue(gc_ptr(String) predicate) {
  return SelectValue(predicate, TimeUtils::DurationToTimeSpan(native::DEFAULT_QUERY_RESPONSE_TIMEOUT));
}

GENERIC(class TKey, class TValue)
gc_ptr(Object) Region<TKey, TValue>::SelectValue(gc_ptr(String) predicate, TimeSpan timeout) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeptr = m_nativeptr->get()->selectValue(
          marshal_as<std::string>(predicate), TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeout));
      return TypeRegistry::GetManagedValueGeneric<gc_ptr(Object)>(nativeptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(ISubscriptionService<TKey>) Region<TKey, TValue>::GetSubscriptionService() {
  return (gc_ptr(ISubscriptionService<TKey>))this;
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) Region<TKey, TValue>::GetLocalView() {
  return gcnew LocalRegion<TKey, TValue>(GetNative());
}
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
