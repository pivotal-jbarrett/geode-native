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

#include "RegionAttributesFactory.hpp"
#include "Region.hpp"
#include "impl/ManagedCacheLoader.hpp"
#include "impl/ManagedPersistenceManager.hpp"
#include "impl/ManagedCacheWriter.hpp"
#include "impl/ManagedCacheListener.hpp"
#include "impl/ManagedPartitionResolver.hpp"
#include "impl/ManagedFixedPartitionResolver.hpp"
#include "impl/CacheLoader.hpp"
#include "impl/CacheWriter.hpp"
#include "impl/CacheListener.hpp"
#include "impl/PartitionResolver.hpp"
#include "impl/PersistenceManagerProxy.hpp"
#include "RegionAttributes.hpp"
#include "ICacheLoader.hpp"
#include "IPersistenceManager.hpp"
#include "ICacheWriter.hpp"
#include "IPartitionResolver.hpp"
#include "IFixedPartitionResolver.hpp"
#include "impl/SafeConvert.hpp"
#include "ExceptionTypes.hpp"
#include "TimeUtils.hpp"

#include "begin_native.hpp"
#include <memory>
#include "end_native.hpp"

namespace Apache {
namespace Geode {
namespace Client {
using namespace System;
using namespace System::Collections::Generic;
using namespace msclr::interop;

namespace native = apache::geode::client;

GENERIC(class TKey, class TValue)
RegionAttributesFactory<TKey, TValue>::RegionAttributesFactory(
    gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) regionAttributes) {
  auto attribptr = regionAttributes->GetNative();
  m_nativeptr = gcnew native_unique_ptr<native::RegionAttributesFactory>(
      std::make_unique<native::RegionAttributesFactory>(*attribptr));
}

// CALLBACKS

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCacheLoader(gc_ptr(ICacheLoader<TKey, TValue>) cacheLoader) {
  std::shared_ptr<native::CacheLoader> loaderptr;
  if (cacheLoader != nullptr) {
    gc_ptr(CacheLoaderGeneric<TKey, TValue>) clg = gcnew CacheLoaderGeneric<TKey, TValue>();
    clg->SetCacheLoader(cacheLoader);
    loaderptr = std::shared_ptr<native::ManagedCacheLoaderGeneric>(new native::ManagedCacheLoaderGeneric(cacheLoader));
    ((native::ManagedCacheLoaderGeneric*)loaderptr.get())->setptr(clg);
  }
  try {
    m_nativeptr->get()->setCacheLoader(loaderptr);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCacheWriter(gc_ptr(ICacheWriter<TKey, TValue>) cacheWriter) {
  std::shared_ptr<native::CacheWriter> writerptr;
  if (cacheWriter != nullptr) {
    gc_ptr(CacheWriterGeneric<TKey, TValue>) cwg = gcnew CacheWriterGeneric<TKey, TValue>();
    cwg->SetCacheWriter(cacheWriter);
    writerptr = std::shared_ptr<native::ManagedCacheWriterGeneric>(new native::ManagedCacheWriterGeneric(cacheWriter));
    ((native::ManagedCacheWriterGeneric*)writerptr.get())->setptr(cwg);
  }
  try {
    m_nativeptr->get()->setCacheWriter(writerptr);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCacheListener(gc_ptr(ICacheListener<TKey, TValue>) cacheListener) {
  std::shared_ptr<native::CacheListener> listenerptr;
  if (cacheListener != nullptr) {
    gc_ptr(CacheListenerGeneric<TKey, TValue>) clg = gcnew CacheListenerGeneric<TKey, TValue>();
    clg->SetCacheListener(cacheListener);
    listenerptr =
        std::shared_ptr<native::ManagedCacheListenerGeneric>(new native::ManagedCacheListenerGeneric(cacheListener));
    ((native::ManagedCacheListenerGeneric*)listenerptr.get())->setptr(clg);
  }
  try {
    m_nativeptr->get()->setCacheListener(listenerptr);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPartitionResolver(
    gc_ptr(IPartitionResolver<TKey, TValue>) partitionresolver) {
  std::shared_ptr<native::PartitionResolver> resolverptr;
  if (partitionresolver != nullptr) {
    gc_ptr(Client::IFixedPartitionResolver<TKey, TValue>) resolver =
        dynamic_cast<gc_ptr(Client::IFixedPartitionResolver<TKey, TValue>)>(partitionresolver);
    if (resolver != nullptr) {
      gc_ptr(FixedPartitionResolverGeneric<TKey, TValue>) prg = gcnew FixedPartitionResolverGeneric<TKey, TValue>();
      prg->SetPartitionResolver(partitionresolver);
      resolverptr = std::shared_ptr<native::ManagedFixedPartitionResolverGeneric>(
          new native::ManagedFixedPartitionResolverGeneric(partitionresolver));
      ((native::ManagedFixedPartitionResolverGeneric*)resolverptr.get())->setptr(prg);
    } else {
      gc_ptr(PartitionResolverGeneric<TKey, TValue>) prg = gcnew PartitionResolverGeneric<TKey, TValue>();
      prg->SetPartitionResolver(partitionresolver);
      resolverptr = std::shared_ptr<native::ManagedPartitionResolverGeneric>(
          new native::ManagedPartitionResolverGeneric(partitionresolver));
      ((native::ManagedPartitionResolverGeneric*)resolverptr.get())->setptr(prg);
    }
  }
  try {
    m_nativeptr->get()->setPartitionResolver(resolverptr);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCacheLoader(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheLoader(marshal_as<std::string>(libPath), marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCacheWriter(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheWriter(marshal_as<std::string>(libPath), marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetCacheListener(
    gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheListener(marshal_as<std::string>(libPath),
                                         marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPartitionResolver(
    gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setPartitionResolver(marshal_as<std::string>(libPath),
                                             marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

// EXPIRATION ATTRIBUTES

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetEntryIdleTimeout(ExpirationAction action, TimeSpan idleTimeout) {
  try {
    m_nativeptr->get()->setEntryIdleTimeout(static_cast<native::ExpirationAction>(action),
                                            TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(idleTimeout));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetEntryTimeToLive(ExpirationAction action, TimeSpan timeToLive) {
  try {
    m_nativeptr->get()->setEntryTimeToLive(static_cast<native::ExpirationAction>(action),
                                           TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeToLive));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetRegionIdleTimeout(ExpirationAction action, TimeSpan idleTimeout) {
  try {
    m_nativeptr->get()->setRegionIdleTimeout(static_cast<native::ExpirationAction>(action),
                                             TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(idleTimeout));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetRegionTimeToLive(ExpirationAction action, TimeSpan timeToLive) {
  try {
    m_nativeptr->get()->setRegionTimeToLive(static_cast<native::ExpirationAction>(action),
                                            TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeToLive));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

// PERSISTENCE
GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPersistenceManager(
    gc_ptr(IPersistenceManager<TKey, TValue>) persistenceManager,
    gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) config) {
  std::shared_ptr<native::PersistenceManager> persistenceManagerptr;
  if (persistenceManager != nullptr) {
    gc_ptr(PersistenceManagerGeneric<TKey, TValue>) clg = gcnew PersistenceManagerGeneric<TKey, TValue>();
    clg->SetPersistenceManager(persistenceManager);
    persistenceManagerptr = std::shared_ptr<native::ManagedPersistenceManagerGeneric>(
        new native::ManagedPersistenceManagerGeneric(persistenceManager));
    ((native::ManagedPersistenceManagerGeneric*)persistenceManagerptr.get())->setptr(clg);
  }
  try {
    m_nativeptr->get()->setPersistenceManager(persistenceManagerptr, config->GetNative());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPersistenceManager(
    gc_ptr(IPersistenceManager<TKey, TValue>) persistenceManager) {
  SetPersistenceManager(persistenceManager, nullptr);
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPersistenceManager(
    gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  SetPersistenceManager(libPath, factoryFunctionName, nullptr);
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPersistenceManager(
    gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName,
    gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) config) {
  try {
    m_nativeptr->get()->setPersistenceManager(marshal_as<std::string>(libPath),
                                              marshal_as<std::string>(factoryFunctionName), config->GetNative());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

// STORAGE ATTRIBUTES

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetPoolName(gc_ptr(String)
                                                                                                     poolName) {
  try {
    m_nativeptr->get()->setPoolName(marshal_as<std::string>(poolName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

// MAP ATTRIBUTES

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetInitialCapacity(System::Int32 initialCapacity) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->setInitialCapacity(initialCapacity);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::SetLoadFactor(Single loadFactor) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->setLoadFactor(loadFactor);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetConcurrencyLevel(System::Int32 concurrencyLevel) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      m_nativeptr->get()->setConcurrencyLevel(concurrencyLevel);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */

  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetLruEntriesLimit(System::UInt32 entriesLimit) {
  try {
    m_nativeptr->get()->setLruEntriesLimit(entriesLimit);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetDiskPolicy(DiskPolicyType diskPolicy) {
  try {
    m_nativeptr->get()->setDiskPolicy(static_cast<native::DiskPolicyType>(diskPolicy));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCachingEnabled(bool cachingEnabled) {
  try {
    m_nativeptr->get()->setCachingEnabled(cachingEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetCloningEnabled(bool cloningEnabled) {
  try {
    m_nativeptr->get()->setCloningEnabled(cloningEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionAttributesFactory<TKey, TValue>)
    RegionAttributesFactory<TKey, TValue>::SetConcurrencyChecksEnabled(bool concurrencyChecksEnabled) {
  try {
    m_nativeptr->get()->setConcurrencyChecksEnabled(concurrencyChecksEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}
// FACTORY METHOD

GENERIC(class TKey, class TValue)
gc_ptr(Apache::Geode::Client::RegionAttributes<TKey, TValue>) RegionAttributesFactory<TKey, TValue>::Create() {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeRegionAttributes = m_nativeptr->get()->create();
      return Apache::Geode::Client::RegionAttributes<TKey, TValue>::Create(nativeRegionAttributes);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
