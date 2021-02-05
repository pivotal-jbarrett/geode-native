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

#include "geode_defs.hpp"
#include "begin_native.hpp"
#include <geode/Cache.hpp>
#include "end_native.hpp"

#include "native_shared_ptr.hpp"
#include "IRegion.hpp"
#include "Log.hpp"
#include "ExceptionTypes.hpp"
#include "RegionAttributes.hpp"

using namespace System;
using namespace System::Collections::Generic;

namespace Apache {
namespace Geode {
namespace Client {

namespace native = apache::geode::client;

GENERIC(class TKey, class TValue)
ref class RegionEntry;

GENERIC(class TKey, class TValue)
ref class AttributesMutator;

GENERIC(class TKey, class TValue)
PUBLIC ref class LocalRegion : public IRegion<TKey, TValue> {
 public:
  virtual property TValue default [TKey] {
    TValue get(TKey key);
    void set(TKey key, TValue value);
  }

      virtual System::Collections::Generic::IEnumerator<KeyValuePair<TKey, TValue>> ^
      GetEnumerator();

  virtual gc_ptr(System::Collections::IEnumerator) GetEnumeratorOld() = System::Collections::IEnumerable::GetEnumerator;

  virtual bool ContainsKey(TKey key);

  virtual void Add(TKey key, TValue val);

  virtual void Add(KeyValuePair<TKey, TValue> keyValuePair);

  virtual void Add(TKey key, TValue value, gc_ptr(Object) callbackArg);

  virtual bool Remove(TKey key);

  virtual bool Remove(TKey key, gc_ptr(Object) callbackArg);

  virtual bool Remove(KeyValuePair<TKey, TValue> keyValuePair);

  virtual bool Remove(TKey key, TValue value, gc_ptr(Object) callbackArg);

  virtual bool Contains(KeyValuePair<TKey, TValue> keyValuePair);

  virtual void Clear();

  virtual void Clear(gc_ptr(Object) callbackArg);

  virtual void CopyTo(array<KeyValuePair<TKey, TValue>> ^ toArray, int startIdx);

  virtual bool TryGetValue(TKey key, TValue % val);

  virtual property int Count { int get(); }

  virtual property bool IsReadOnly {
    bool get() { throw gcnew System::NotImplementedException; /*return false;*/ }
  }

  virtual property gc_ptr(System::Collections::Generic::ICollection<TKey>) Keys {
    gc_ptr(System::Collections::Generic::ICollection<TKey>) get();
  }

  virtual property gc_ptr(System::Collections::Generic::ICollection<TValue>) Values {
    gc_ptr(System::Collections::Generic::ICollection<TValue>) get();
  }

  virtual void Put(TKey key, TValue value, gc_ptr(Object) callbackArg);

  virtual void Put(TKey key, TValue value);

  virtual TValue Get(TKey key, gc_ptr(Object) callbackArg);

  virtual TValue Get(TKey key);

  virtual void InvalidateRegion();

  virtual void InvalidateRegion(gc_ptr(Object) callbackArg);

  virtual void DestroyRegion();

  virtual void DestroyRegion(gc_ptr(Object) callbackArg);

  virtual void Invalidate(TKey key);

  virtual void Invalidate(TKey key, gc_ptr(Object) callbackArg);

  virtual void PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map);

  virtual void PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map, TimeSpan timeout);

  virtual void PutAll(gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) map, TimeSpan timeout,
                      gc_ptr(Object) callbackArg);

  virtual void GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions);

  virtual void GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions,
                      bool addToLocalCache);

  virtual void GetAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, TValue>) values,
                      gc_ptr(System::Collections::Generic::IDictionary<TKey, gc_ptr(System::Exception)>) exceptions,
                      bool addToLocalCache, gc_ptr(Object) callbackArg);

  virtual void RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys);
  virtual void RemoveAll(gc_ptr(System::Collections::Generic::ICollection<TKey>) keys, gc_ptr(Object) callbackArg);

  virtual property gc_ptr(String) Name { gc_ptr(String) get(); }

  virtual property gc_ptr(String) FullPath { gc_ptr(String) get(); }

  virtual property gc_ptr(IRegion<TKey, TValue>) ParentRegion { gc_ptr(IRegion<TKey, TValue>) get(); }

  virtual property gc_ptr(RegionAttributes<TKey, TValue>) Attributes { gc_ptr(RegionAttributes<TKey, TValue>) get(); }

  virtual property gc_ptr(AttributesMutator<TKey, TValue>) AttributesMutator {
    gc_ptr(Apache::Geode::Client::AttributesMutator<TKey, TValue>) get();
  }

  virtual property gc_ptr(Apache::Geode::Client::CacheStatistics) Statistics {
    gc_ptr(Apache::Geode::Client::CacheStatistics) get();
  }

  virtual gc_ptr(IRegion<TKey, TValue>) GetSubRegion(gc_ptr(String) path);

  virtual gc_ptr(IRegion<TKey, TValue>)
      CreateSubRegion(gc_ptr(String) subRegionName, gc_ptr(RegionAttributes<TKey, TValue>) attributes);

  virtual System::Collections::Generic::ICollection<gc_ptr(IRegion<TKey, TValue>)> ^ SubRegions(bool recursive);

  virtual gc_ptr(RegionEntry<TKey, TValue>) GetEntry(TKey key);

  virtual System::Collections::Generic::ICollection<gc_ptr(RegionEntry<TKey, TValue>)> ^ GetEntries(bool recursive);

  virtual property gc_ptr(Apache::Geode::Client::IRegionService) RegionService {
    gc_ptr(Apache::Geode::Client::IRegionService) get();
  }

  virtual bool ContainsValueForKey(TKey key);

  // Additional Region properties and methods
  virtual property bool IsDestroyed { bool get(); }

  GENERIC(class TResult)
  virtual gc_ptr(ISelectResults<TResult>) Query(gc_ptr(String) predicate);

  GENERIC(class TResult)
  virtual gc_ptr(ISelectResults<TResult>) Query(gc_ptr(String) predicate, TimeSpan timeout);

  virtual bool ExistsValue(gc_ptr(String) predicate);

  virtual bool ExistsValue(gc_ptr(String) predicate, TimeSpan timeout);

  virtual gc_ptr(Object) SelectValue(gc_ptr(String) predicate);

  virtual gc_ptr(Object) SelectValue(gc_ptr(String) predicate, TimeSpan timeout);

  virtual gc_ptr(ISubscriptionService<TKey>) GetSubscriptionService();

  virtual gc_ptr(IRegion<TKey, TValue>) GetLocalView();

  CLI(internal:)
  /// <summary>
  /// Internal factory function to wrap a native object pointer inside
  /// this managed class with null pointer check.
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  /// <returns>
  /// The managed wrapper object; null if the native pointer is null.
  /// </returns>
  // GENERIC(class TKey, class TValue)
  inline static gc_ptr(IRegion<TKey, TValue>) Create(std::shared_ptr<native::Region> nativeptr) {
    return __nullptr == nativeptr ? nullptr : gcnew LocalRegion<TKey, TValue>(nativeptr);
  }

  std::shared_ptr<native::Region> GetNative() { return m_nativeptr->get_shared_ptr(); }

  /// <summary>
  /// Private constructor to wrap a native object pointer
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  inline LocalRegion(std::shared_ptr<native::Region> nativeptr) {
    m_nativeptr = gcnew native_shared_ptr<native::Region>(nativeptr);
  }

 private:
  inline std::shared_ptr<apache::geode::client::Serializable> getRegionEntryValue(
      std::shared_ptr<apache::geode::client::CacheableKey>& key);

  gc_ptr(native_shared_ptr<native::Region>) m_nativeptr;
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
