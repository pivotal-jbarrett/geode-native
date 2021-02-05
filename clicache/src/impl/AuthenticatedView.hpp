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

#include "../geode_defs.hpp"
#include "../begin_native.hpp"
#include <geode/RegionService.hpp>
#include <geode/AuthenticatedView.hpp>
#include "../end_native.hpp"

#include "../native_unique_ptr.hpp"
#include "../RegionShortcut.hpp"
#include "../RegionFactory.hpp"
#include "../IRegionService.hpp"
#include "../Region.hpp"

using namespace System;
using namespace System::Collections::Generic;
namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

/// <summary>
/// Provides a distributed cache.
/// </summary>
/// <remarks>
/// Caches are obtained from static methods on the
/// <see cref="CacheFactory"/> class.
/// <para>
/// When a cache is created a <see cref="DistributedSystem" />
/// must be specified.
/// </para><para>
/// When a cache will no longer be used, call <see cref="Cache.Close" />.
/// Once it <see cref="Cache.IsClosed" /> any attempt to use it
/// will cause a <c>CacheClosedException</c> to be thrown.
/// </para><para>
/// A cache can have multiple root regions, each with a different name.
/// </para>
/// </remarks>
PUBLIC ref class AuthenticatedView : public IRegionService {
 public:
  /// <summary>
  /// True if this cache has been closed.
  /// </summary>
  /// <remarks>
  /// After a new cache object is created, this method returns false.
  /// After <see cref="Close" /> is called on this cache object, this method
  /// returns true.
  /// </remarks>
  /// <returns>true if this cache is closed, otherwise false</returns>
  virtual property bool IsClosed { bool get(); }

  /// <summary>
  /// Terminates this object cache and releases all the local resources.
  /// If Cache instance created from Pool(pool is in multiuser mode), then it reset user related security data.
  /// </summary>
  /// <remarks>
  /// After this cache is closed, any further
  /// method call on this cache or any region object will throw
  /// <c>CacheClosedException</c>, unless otherwise noted.
  /// </remarks>
  /// <exception cref="CacheClosedException">
  /// if the cache is already closed.
  /// </exception>
  virtual void Close();

  /// <summary>
  /// Returns an existing region given the full path from root, or null
  /// if no such region exists.
  /// </summary>
  /// <remarks>
  /// If Pool attached with Region is in multiusersecure mode then don't use return instance of region as no credential
  /// are attached with this instance. Get logical instance of region
  /// Pool->CreateSecureUserCache(<Credential>).getRegion(<name>) to do the operation on Cache.
  /// </remarks>
  /// <param name="path">the pathname of the region</param>
  /// <returns>the region</returns>
  GENERIC(class TKey, class TValue)
  virtual gc_ptr(IRegion<TKey, TValue>) GetRegion(gc_ptr(String) path);

  /// <summary>
  /// Get a query service object to be able to query the cache.
  /// Supported only when cache is created from Pool(pool is in multiuserSecure mode)
  /// </summary>
  /// <remarks>
  /// Currently only works against the java server in native mode, and
  /// at least some endpoints must have been defined in some regions
  /// before actually firing a query.
  /// </remarks>
  virtual gc_ptr(QueryService) GetQueryService();

  /// <summary>
  /// Returns an array of root regions in the cache. This set is a
  /// snapshot and is not backed by the cache.
  /// </summary>
  /// <remarks>
  /// It is not supported when Cache is created from Pool.
  /// </remarks>
  /// <returns>array of regions</returns>
  GENERIC(class TKey, class TValue)
  virtual array<gc_ptr(IRegion<TKey, TValue>)> ^ RootRegions();

  /// <summary>
  /// Returns a factory that can create a {@link PdxInstance}.
  /// @param className the fully qualified class name that the PdxInstance will become
  ///   when it is fully deserialized.
  /// @return the factory
  /// </summary>
  virtual gc_ptr(IPdxInstanceFactory) CreatePdxInstanceFactory(gc_ptr(String) className);

  CLI(internal:)

  /// <summary>
  /// Internal factory function to wrap a native object pointer inside
  /// this managed class with null pointer check.
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  /// <returns>
  /// The managed wrapper object; null if the native pointer is null.
  /// </returns>
  inline static gc_ptr(AuthenticatedView) Create(native::AuthenticatedView&& nativeAuthenticatedView) {
    return gcnew AuthenticatedView(std::move(nativeAuthenticatedView));
  }

  native::AuthenticatedView& GetNative() { return *(m_nativeptr->get()); }

 private:
  /// <summary>
  /// Private constructor to wrap a native object pointer
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  inline AuthenticatedView(native::AuthenticatedView&& nativeAuthenticatedView) {
    m_nativeptr = gcnew native_unique_ptr<native::AuthenticatedView>(
        std::unique_ptr<native::AuthenticatedView>(new native::AuthenticatedView(std::move(nativeAuthenticatedView))));
  }

  gc_ptr(native_unique_ptr<native::AuthenticatedView>) m_nativeptr;
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
