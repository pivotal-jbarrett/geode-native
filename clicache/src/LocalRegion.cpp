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
#include "geode/Region.hpp"
#include "geode/Cache.hpp"
#include "end_native.hpp"

#include "LocalRegion.hpp"
#include "Cache.hpp"
#include "CacheStatistics.hpp"
#include "AttributesMutator.hpp"
#include "RegionEntry.hpp"
#include "impl/AuthenticatedView.hpp"
#include "impl/SafeConvert.hpp"
#include "impl/CacheResolver.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {
using namespace msclr::interop;

GENERIC(class TKey, class TValue)
TValue LocalRegion<TKey, TValue>::Get(TKey key, gc_ptr(Object) callbackArg) {
  auto keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
  auto nativeptr = this->getRegionEntryValue(keyptr);
  if (nativeptr == nullptr) {
    throw gcnew KeyNotFoundException("The given key was not present in the region");
  }
  TValue returnVal = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return returnVal;
}

GENERIC(class TKey, class TValue)
TValue LocalRegion<TKey, TValue>::Get(TKey key) { return Get(key, nullptr); }

GENERIC(class TKey, class TValue)
std::shared_ptr<apache::geode::client::Serializable> LocalRegion<TKey, TValue>::getRegionEntryValue(
    std::shared_ptr<apache::geode::client::CacheableKey>& keyptr) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      if (auto entryPtr = m_nativeptr->get()->getEntry(keyptr)) {
        return entryPtr->getValue();
      } else {
        return nullptr;
      }
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Put(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localPut(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Put(TKey key, TValue value) { Put(key, value, nullptr); }

GENERIC(class TKey, class TValue)
TValue LocalRegion<TKey, TValue>::default ::get(TKey key) {
  std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
  auto nativeptr = this->getRegionEntryValue(keyptr);
  if (nativeptr == nullptr) {
    throw gcnew KeyNotFoundException("The given key was not present in the region");
  }
  TValue returnVal = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return returnVal;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::default ::set(TKey key, TValue value) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      m_nativeptr->get()->localPut(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
System::Collections::Generic::IEnumerator<KeyValuePair<TKey, TValue>> ^ LocalRegion<TKey, TValue>::GetEnumerator() {
  std::vector<std::shared_ptr<apache::geode::client::RegionEntry>> vc;

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  auto toArray = gcnew array<KeyValuePair<TKey, TValue>>(static_cast<int>(vc.size()));
  for (System::Int32 index = 0; index < vc.size(); index++) {
    auto nativeptr = vc[index];
    TKey key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
    TValue val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
    toArray[index] = KeyValuePair<TKey, TValue>(key, val);
  }
  return ((System::Collections::Generic::IEnumerable<KeyValuePair<TKey, TValue>> ^) toArray)->GetEnumerator();
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::IEnumerator) LocalRegion<TKey, TValue>::GetEnumeratorOld() {
  std::vector<std::shared_ptr<apache::geode::client::RegionEntry>> vc;

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  auto toArray = gcnew array<gc_ptr(Object)>(static_cast<int>(vc.size()));
  for (System::Int32 index = 0; index < vc.size(); index++) {
    auto nativeptr = vc[index];
    TKey key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
    TValue val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
    toArray[index] = KeyValuePair<TKey, TValue>(key, val);
  }
  return ((gc_ptr(System::Collections::Generic::IEnumerable<gc_ptr(Object)>))toArray)->GetEnumerator();
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::Contains(KeyValuePair<TKey, TValue> keyValuePair) {
  auto keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
  auto nativeptr = this->getRegionEntryValue(keyptr);
  // This means that key is not present.
  if (nativeptr == nullptr) {
    return false;
  }
  TValue value = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
  return ((gc_ptr(Object))value)->Equals(keyValuePair.Value);
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::ContainsKey(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      return m_nativeptr->get()->containsKey(keyptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::TryGetValue(TKey key, TValue % val) {
  auto keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
  auto nativeptr = this->getRegionEntryValue(keyptr);
  if (nativeptr == nullptr) {
    val = TValue();
    return false;
  } else {
    val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr);
    return true;
  }
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<TKey>) LocalRegion<TKey, TValue>::Keys::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<apache::geode::client::CacheableKey>> vc;
    try {
      vc = m_nativeptr->get()->keys();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

    auto keyarr = gcnew array<TKey>(static_cast<int>(vc.size()));
    for (System::Int32 index = 0; index < vc.size(); index++) {
      auto& nativeptr = vc[index];
      keyarr[index] = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr);
    }
    auto collectionlist = (gc_ptr(System::Collections::Generic::ICollection<TKey>))keyarr;
    return collectionlist;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(System::Collections::Generic::ICollection<TValue>) LocalRegion<TKey, TValue>::Values::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<apache::geode::client::Cacheable>> vc;
    try {
      vc = m_nativeptr->get()->values();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

    // gc_ptr(List<TValue>) collectionlist = gcnew List<TValue>(vc.size());
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
void LocalRegion<TKey, TValue>::Add(TKey key, TValue value) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      m_nativeptr->get()->localCreate(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Add(KeyValuePair<TKey, TValue> keyValuePair) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValuePair.Value);
      m_nativeptr->get()->localCreate(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Add(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localCreate(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::Remove(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      m_nativeptr->get()->localDestroy(keyptr);
      return true;
    } catch (apache::geode::client::EntryNotFoundException /*ex*/) {
      return false;
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::Remove(TKey key, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localDestroy(keyptr, callbackptr);
      return true;
    } catch (apache::geode::client::EntryNotFoundException /*ex*/) {
      return false;
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::Remove(KeyValuePair<TKey, TValue> keyValuePair) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(keyValuePair.Key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(keyValuePair.Value);
      return m_nativeptr->get()->localRemove(keyptr, valueptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::Remove(TKey key, TValue value, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Cacheable> valueptr = Serializable::GetUnmanagedValueGeneric<TValue>(value);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      return m_nativeptr->get()->localRemove(keyptr, valueptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::InvalidateRegion() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    InvalidateRegion(nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::InvalidateRegion(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localInvalidateRegion(callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::DestroyRegion() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    DestroyRegion(nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::DestroyRegion(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localDestroyRegion(callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Invalidate(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    Invalidate(key, nullptr);

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Invalidate(TKey key, gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      std::shared_ptr<native::CacheableKey> keyptr = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      std::shared_ptr<native::Serializable> callbackptr =
          Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg);
      m_nativeptr->get()->localInvalidate(keyptr, callbackptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map,
                                       TimeSpan timeout) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map,
                                       TimeSpan timeout, gc_ptr(Object) callbackArg) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::GetAll(
    gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::GetAll(
    gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions,
    bool addToLocalCache) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::GetAll(
    gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
    gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions, bool addToLocalCache,
    gc_ptr(Object) callbackArg) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                                          gc_ptr(Object) callbackArg) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
gc_ptr(String) LocalRegion<TKey, TValue>::Name::get() {
  try {
    return marshal_as<gc_ptr(String)>(m_nativeptr->get()->getName());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
gc_ptr(String) LocalRegion<TKey, TValue>::FullPath::get() {
  try {
    return marshal_as<gc_ptr(String)>(m_nativeptr->get()->getFullPath());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) LocalRegion<TKey, TValue>::ParentRegion::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto parentRegion = m_nativeptr->get()->getParentRegion();
      auto region = Region<TKey, TValue>::Create(parentRegion);
      if (region == nullptr) {
        return nullptr;
      }
      return region->GetLocalView();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) LocalRegion<TKey, TValue>::Attributes::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      return Apache::Geode::Client::RegionAttributes<TKey, TValue>::Create(m_nativeptr->get()->getAttributes());
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(AttributesMutator<TKey, TValue>) LocalRegion<TKey, TValue>::AttributesMutator::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      return Apache::Geode::Client::AttributesMutator<TKey, TValue>::Create(m_nativeptr->get()->getAttributesMutator());
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(Apache::Geode::Client::CacheStatistics) LocalRegion<TKey, TValue>::Statistics::get() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      return Apache::Geode::Client::CacheStatistics::Create(m_nativeptr->get()->getStatistics());
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) LocalRegion<TKey, TValue>::GetSubRegion(gc_ptr(String) path) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeptr = m_nativeptr->get()->getSubregion(marshal_as<std::string>(path));
      auto region = Region<TKey, TValue>::Create(nativeptr);
      if (region == nullptr) {
        return nullptr;
      }
      return region->GetLocalView();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) LocalRegion<TKey, TValue>::CreateSubRegion(
    gc_ptr(String) subRegionName, gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) attributes) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      native::RegionAttributes regionAttributes;
      return Region<TKey, TValue>::Create(
                 m_nativeptr->get()->createSubregion(marshal_as<std::string>(subRegionName), regionAttributes))
          ->GetLocalView();
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
System::Collections::Generic::ICollection<gc_ptr(IRegion<TKey, TValue>)> ^
    LocalRegion<TKey, TValue>::SubRegions(bool recursive) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<apache::geode::client::Region>> vsr;
    try {
      vsr = m_nativeptr->get()->subregions(recursive);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    array<gc_ptr(IRegion<TKey, TValue>)> ^ subRegions =
        gcnew array<gc_ptr(IRegion<TKey, TValue>)>(static_cast<int>(vsr.size()));

    for (System::Int32 index = 0; index < vsr.size(); index++) {
      auto nativeptr = vsr[index];
      subRegions[index] = Region<TKey, TValue>::Create(nativeptr)->GetLocalView();
    }
    auto collection = (System::Collections::Generic::ICollection<gc_ptr(IRegion<TKey, TValue>)> ^) subRegions;
    return collection;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionEntry<TKey, TValue>) LocalRegion<TKey, TValue>::GetEntry(TKey key) {
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
    LocalRegion<TKey, TValue>::GetEntries(bool recursive) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<apache::geode::client::RegionEntry>> vc;
    try {
      vc = m_nativeptr->get()->entries(recursive);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    auto entryarr = gcnew array<gc_ptr(RegionEntry<TKey, TValue>)>(static_cast<int>(vc.size()));

    for (System::Int32 index = 0; index < vc.size(); index++) {
      auto nativeptr = vc[index];
      entryarr[index] = RegionEntry<TKey, TValue>::Create(nativeptr);
    }
    auto collection = (System::Collections::Generic::ICollection<gc_ptr(RegionEntry<TKey, TValue>)> ^) entryarr;
    return collection;

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegionService) LocalRegion<TKey, TValue>::RegionService::get() {
  return CacheResolver::Lookup(&m_nativeptr->get()->getCache());
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::ContainsValueForKey(TKey key) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto v = Serializable::GetUnmanagedValueGeneric<TKey>(key);
      return m_nativeptr->get()->containsValueForKey(v);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
int LocalRegion<TKey, TValue>::Count::get() {
  try {
    return m_nativeptr->get()->size();
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Clear() { Clear(nullptr); }

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::Clear(gc_ptr(Object) callbackArg) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */
    try {
      m_nativeptr->get()->localClear(Serializable::GetUnmanagedValueGeneric<gc_ptr(Object)>(callbackArg));
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
void LocalRegion<TKey, TValue>::CopyTo(array<KeyValuePair<TKey, TValue>> ^ toArray, int startIdx) {
  if (toArray == nullptr) {
    throw gcnew System::ArgumentNullException;
  }
  if (startIdx < 0) {
    throw gcnew System::ArgumentOutOfRangeException;
  }

  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    std::vector<std::shared_ptr<apache::geode::client::RegionEntry>> vc;
    try {
      vc = m_nativeptr->get()->entries(false);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

    if (toArray->Rank > 1 || (vc.size() > (toArray->Length - startIdx))) {
      throw gcnew System::ArgumentException;
    }

    for (System::Int32 index = 0; index < vc.size(); index++) {
      std::shared_ptr<apache::geode::client::RegionEntry> nativeptr = vc[index];
      TKey key = TypeRegistry::GetManagedValueGeneric<TKey>(nativeptr->getKey());
      TValue val = TypeRegistry::GetManagedValueGeneric<TValue>(nativeptr->getValue());
      toArray[startIdx] = KeyValuePair<TKey, TValue>(key, val);
      ++startIdx;
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::IsDestroyed::get() {
  try {
    return m_nativeptr->get()->isDestroyed();
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
}

GENERIC(class TKey, class TValue)
GENERIC(class TResult)
gc_ptr(ISelectResults<TResult>) LocalRegion<TKey, TValue>::Query(gc_ptr(String) predicate) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
GENERIC(class TResult)
gc_ptr(ISelectResults<TResult>) LocalRegion<TKey, TValue>::Query(gc_ptr(String) predicate, TimeSpan timeout) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::ExistsValue(gc_ptr(String) predicate) { throw gcnew System::NotSupportedException; }

GENERIC(class TKey, class TValue)
bool LocalRegion<TKey, TValue>::ExistsValue(gc_ptr(String) predicate, TimeSpan timeout) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
gc_ptr(Object) LocalRegion<TKey, TValue>::SelectValue(gc_ptr(String) predicate) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
gc_ptr(Object) LocalRegion<TKey, TValue>::SelectValue(gc_ptr(String) predicate, TimeSpan timeout) {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
gc_ptr(ISubscriptionService<TKey>) LocalRegion<TKey, TValue>::GetSubscriptionService() {
  throw gcnew System::NotSupportedException;
}

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) LocalRegion<TKey, TValue>::GetLocalView() { throw gcnew System::NotSupportedException; }
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
