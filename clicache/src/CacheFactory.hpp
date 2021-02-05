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
#include <geode/CacheFactory.hpp>
#include "end_native.hpp"

#include "native_conditional_shared_ptr.hpp"
#include "IAuthInitialize.hpp"

using namespace System::Collections::Generic;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

ref class Cache;
ref class CacheAttributes;
ref class DistributedSystem;

/// <summary>
/// A factory class that must be used to obtain instance of <see cref="Cache" />.
/// </summary>
/// <remarks>
/// To create a new cache instance, use <see cref="CacheFactory.Create" />.
/// <para>
/// To get an existing unclosed cache instance, use <see cref="Cache.GetInstance" />.
/// </para>
/// </remarks>
PUBLIC ref class CacheFactory {
 public:
  /// <summary>
  /// A factory class that must be used to obtain instance of <see cref="Cache" />.
  /// </summary>
  /// <param name="dsProps">Properties which are applicable at client level.</param>
  CacheFactory(gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) dsProps);

  /// <summary>
  /// A factory class that must be used to obtain instance of <see cref="Cache" />.
  /// </summary>
  CacheFactory();

  /// <summary>
  /// To create the instance of <see cref="Cache" />.
  /// </summary>
  gc_ptr(Cache) Create();

  /// <summary>
  /// Returns the version of the cache implementation.
  /// For the 1.0 release of Geode, the string returned is <c>1.0</c>.
  /// </summary>
  /// <returns>the version of the cache implementation as a <c>String</c></returns>
  static property gc_ptr(String) Version { static gc_ptr(String) get(); }

  /// <summary>
  /// Returns the product description string including product name and version.
  /// </summary>
  static property gc_ptr(String) ProductDescription { static gc_ptr(String) get(); }

  ///< summary>
  /// Control whether pdx ignores fields that were unread during deserialization.
  /// The default is to preserve unread fields be including their data during serialization.
  /// But if you configure the cache to ignore unread fields then their data will be lost
  /// during serialization.
  /// <P>You should only set this attribute to <code>true</code> if you know this member
  /// will only be reading cache data. In this use case you do not need to pay the cost
  /// of preserving the unread fields since you will never be reserializing pdx data.
  ///< summary>
  /// <param> ignore <code>true</code> if fields not read during pdx deserialization should be ignored;
  /// <code>false</code>, the default, if they should be preserved.
  /// </param>
  /// <returns>
  /// a instance of <c>CacheFactory</c>
  /// </returns>
  gc_ptr(CacheFactory) SetPdxIgnoreUnreadFields(bool ignore);

  ///< summary>
  /// Sets the object preference to PdxInstance type.
  /// When a cached object that was serialized as a PDX is read
  /// from the cache a {@link PdxInstance} will be returned instead of the actual domain class.
  /// The PdxInstance is an interface that provides run time access to
  /// the fields of a PDX without deserializing the entire PDX.
  /// The PdxInstance implementation is a light weight wrapper
  /// that simply refers to the raw bytes of the PDX that are kept
  /// in the cache. Using this method applications can choose to
  /// access PdxInstance instead of Java object.
  /// Note that a PdxInstance is only returned if a serialized PDX is found in the cache.
  /// If the cache contains a deserialized PDX, then a domain class instance is returned instead of a PdxInstance.
  ///</summary>
  /// <param> pdxReadSerialized <code>true</code> to prefer PdxInstance
  /// <code>false</code>, the default, if they should be preserved.
  /// </param>
  /// <returns>
  /// a instance of <c>CacheFactory</c>
  /// </returns>
  gc_ptr(CacheFactory) SetPdxReadSerialized(bool pdxReadSerialized);

  /// <summary>
  /// Sets a geode property that will be used when creating the ClientCache.
  /// </summary>
  /// <param>
  /// name the name of the geode property
  /// </param>
  /// <param>
  /// value the value of the geode property
  /// </param>
  /// <returns>
  /// a instance of <c>CacheFactory</c>
  /// </returns>
  gc_ptr(CacheFactory) Set(gc_ptr(String) name, gc_ptr(String) value);

  /// <summary>
  /// Sets the AuthInitializer defined by the user.
  /// The AuthInitializer will be used to obtain credentials for a client.
  /// </summary>
  /// <param>
  /// authInitialize
  /// </param>
  /// <returns>
  /// a instance of <c>CacheFactory</c>
  /// </returns>
  gc_ptr(CacheFactory) SetAuthInitialize(gc_ptr(IAuthInitialize) authInitialize);

 private:
  /// <summary>
  /// Private constructor to wrap a native object pointer
  /// </summary>
  /// <param name="nativeptr">The native object pointer</param>
  inline CacheFactory(native::CacheFactory nativeptr, gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) dsProps) {
    m_nativeptr = gcnew native_conditional_shared_ptr<native::CacheFactory>(
        std::unique_ptr<native::CacheFactory>(new native::CacheFactory(nativeptr)));
    m_dsProps = dsProps;
  }

  gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) m_dsProps;

  static gc_ptr(System::Object) m_singletonSync = gcnew System::Object();

  gc_ptr(native_conditional_shared_ptr<native::CacheFactory>) m_nativeptr;
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
