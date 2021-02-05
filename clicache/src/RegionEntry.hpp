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
#include <geode/RegionEntry.hpp>
#include "end_native.hpp"

#include "native_shared_ptr.hpp"
//#include "ICacheableKey.hpp"
#include "IRegion.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

// ref class Region;
ref class CacheStatistics;

/// <summary>
/// An object in a region that represents an <em>entry</em>, that is, a key-value pair.
/// </summary>
/// <remarks>
/// This object's operations are not distributed, do not acquire any locks, and do not affect
/// <c>CacheStatistics</c>.
///
/// Unless otherwise noted, all of these methods throw a
/// <c>CacheClosedException</c> if the cache is closed at the time of
/// invocation, or an <c>EntryDestroyedException</c> if the entry has been
/// destroyed.
///
/// Call <see cref="IsDestroyed" /> to see if an entry has already been destroyed.
/// </remarks>
GENERIC(class TKey, class TValue)
PUBLIC ref class RegionEntry sealed {
 public:
  /// <summary>
  /// Returns the key for this entry.
  /// </summary>
  /// <returns>the key for this entry</returns>
  /// <exception cref="CacheClosedException">
  /// if the cache is closed at the time of invocation
  /// </exception>
  /// <exception cref="EntryDestroyedException">
  /// if the entry has been destroyed
  /// </exception>
  property TKey Key { TKey get(); }

  /// <summary>
  /// Returns the value of this entry in the local cache. Does not invoke
  /// an <c>ICacheLoader</c>, does not do a netSearch, netLoad, etc.
  /// </summary>
  /// <returns>
  /// the value, or null if this entry is invalid -- see <see cref="IsDestroyed" />
  /// </returns>
  /// <exception cref="CacheClosedException">
  /// if the cache is closed at the time of invocation
  /// </exception>
  /// <exception cref="EntryDestroyedException">
  /// if the entry has been destroyed
  /// </exception>
  property TValue Value { TValue get(); }

  /// <summary>
  /// Returns the region that contains this entry.
  /// </summary>
  /// <returns>the region that contains this entry</returns>
  /// <exception cref="CacheClosedException">
  /// if the cache is closed at the time of invocation
  /// </exception>
  /// <exception cref="EntryDestroyedException">
  /// if the entry has been destroyed
  /// </exception>
  property gc_ptr(IRegion<TKey, TValue>) Region { gc_ptr(IRegion<TKey, TValue>) get(); }

  /// <summary>
  /// Returns the statistics for this entry.
  /// </summary>
  /// <returns>the CacheStatistics for this entry</returns>
  /// <exception cref="StatisticsDisabledException">
  /// if statistics have been disabled for this region
  /// </exception>
  property gc_ptr(Apache::Geode::Client::CacheStatistics) Statistics {
    gc_ptr(Apache::Geode::Client::CacheStatistics) get();
  }

  ///// <summary>
  ///// Returns the user attribute for this entry in the local cache.
  ///// </summary>
  ///// <returns>the user attribute for this entry</returns>
  ////gc_ptr(Object) GetUserAttribute( );

  ///// <summary>
  ///// Sets the user attribute for this entry. Does not distribute the user
  ///// attribute to other caches.
  ///// </summary>
  ///// <param name="uptr">a pointer to the user attribute for this entry</param>
  ///// <returns>
  ///// the previous user attribute or null if no user attributes have been
  ///// set for this entry
  ///// </returns>
  ////void SetUserAttribute( gc_ptr(Object) uptr );

  /// <summary>
  /// True if this entry has been destroyed.
  /// </summary>
  /// <remarks>
  /// Does not throw a <c>EntryDestroyedException</c> if this entry
  /// has been destroyed.
  /// </remarks>
  /// <returns>true if this entry has been destroyed</returns>
  /// <exception cref="CacheClosedException">
  /// if the cache is closed at the time of invocation
  /// </exception>
  property bool IsDestroyed { bool get(); }

  CLI(internal:)

  /// <summary>
  /// Internal factory function to wrap a native object pointer inside
  /// this managed class with null pointer check.
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  /// <returns>
  /// The managed wrapper object; null if the native pointer is null.
  /// </returns>
  inline static gc_ptr(Client::RegionEntry<TKey, TValue>) Create(std::shared_ptr<native::RegionEntry> nativeptr) {
    return __nullptr == nativeptr ? nullptr : gcnew RegionEntry<TKey, TValue>(nativeptr);
  }

 private:
  /// <summary>
  /// Private constructor to wrap a native object pointer
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  inline RegionEntry(std::shared_ptr<native::RegionEntry> nativeptr) {
    m_nativeptr = gcnew native_shared_ptr<native::RegionEntry>(nativeptr);
  }

  gc_ptr(native_shared_ptr<native::RegionEntry>) m_nativeptr;
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
