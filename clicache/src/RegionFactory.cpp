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

#include "RegionFactory.hpp"
#include "RegionAttributes.hpp"
#include "impl/SafeConvert.hpp"
#include "impl/ManagedCacheLoader.hpp"
#include "impl/ManagedCacheWriter.hpp"
#include "impl/ManagedCacheListener.hpp"
#include "impl/ManagedPartitionResolver.hpp"
#include "impl/ManagedFixedPartitionResolver.hpp"
#include "impl/ManagedFixedPartitionResolver.hpp"
#include "impl/ManagedPersistenceManager.hpp"
#include "impl/CacheLoader.hpp"
#include "impl/CacheWriter.hpp"
#include "impl/CacheListener.hpp"
#include "impl/PartitionResolver.hpp"
#include "impl/FixedPartitionResolver.hpp"
#include "impl/FixedPartitionResolver.hpp"
#include "impl/PersistenceManagerProxy.hpp"
#include "TimeUtils.hpp"

namespace Apache {
namespace Geode {
namespace Client {
using namespace System;
using namespace System::Collections::Generic;

namespace native = apache::geode::client;

gc_ptr(RegionFactory) RegionFactory::SetCacheLoader(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheLoader(marshal_as<std::string>(libPath), marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetCacheWriter(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheWriter(marshal_as<std::string>(libPath), marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetCacheListener(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  throw gcnew System::NotSupportedException;

  try {
    m_nativeptr->get()->setCacheListener(marshal_as<std::string>(libPath),
                                         marshal_as<std::string>(factoryFunctionName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetPartitionResolver(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
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

gc_ptr(RegionFactory) RegionFactory::SetEntryIdleTimeout(ExpirationAction action, TimeSpan idleTimeout) {
  try {
    m_nativeptr->get()->setEntryIdleTimeout(static_cast<native::ExpirationAction>(action),
                                            TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(idleTimeout));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetEntryTimeToLive(ExpirationAction action, TimeSpan timeToLive) {
  try {
    m_nativeptr->get()->setEntryTimeToLive(static_cast<native::ExpirationAction>(action),
                                           TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(timeToLive));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetRegionIdleTimeout(ExpirationAction action, TimeSpan idleTimeout) {
  try {
    m_nativeptr->get()->setRegionIdleTimeout(static_cast<native::ExpirationAction>(action),
                                             TimeUtils::TimeSpanToDurationCeil<std::chrono::seconds>(idleTimeout));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetRegionTimeToLive(ExpirationAction action, TimeSpan timeToLive) {
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
gc_ptr(RegionFactory) RegionFactory::SetPersistenceManager(gc_ptr(Client::IPersistenceManager<TKey, TValue>)
                                                               persistenceManager,
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
gc_ptr(RegionFactory) RegionFactory::SetPersistenceManager(gc_ptr(Client::IPersistenceManager<TKey, TValue>)
                                                               persistenceManager) {
  return SetPersistenceManager(persistenceManager, nullptr);
}

gc_ptr(RegionFactory) RegionFactory::SetPersistenceManager(gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName) {
  SetPersistenceManager(libPath, factoryFunctionName, nullptr);
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetPersistenceManager(
    gc_ptr(String) libPath, gc_ptr(String) factoryFunctionName,
    /*Dictionary<gc_ptr(Object), gc_ptr(Object)>*/ gc_ptr(Properties<gc_ptr(String), gc_ptr(String)>) config) {
  try {
    m_nativeptr->get()->setPersistenceManager(marshal_as<std::string>(libPath),
                                              marshal_as<std::string>(factoryFunctionName), config->GetNative());
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetPoolName(gc_ptr(String) poolName) {
  try {
    m_nativeptr->get()->setPoolName(marshal_as<std::string>(poolName));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

// MAP ATTRIBUTES

gc_ptr(RegionFactory) RegionFactory::SetInitialCapacity(System::Int32 initialCapacity) {
  _GF_MG_EXCEPTION_TRY2

    try {
      m_nativeptr->get()->setInitialCapacity(initialCapacity);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    return this;

  _GF_MG_EXCEPTION_CATCH_ALL2
}

gc_ptr(RegionFactory) RegionFactory::SetLoadFactor(Single loadFactor) {
  _GF_MG_EXCEPTION_TRY2

    try {
      m_nativeptr->get()->setLoadFactor(loadFactor);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    return this;

  _GF_MG_EXCEPTION_CATCH_ALL2
}

gc_ptr(RegionFactory) RegionFactory::SetConcurrencyLevel(System::Int32 concurrencyLevel) {
  _GF_MG_EXCEPTION_TRY2

    try {
      m_nativeptr->get()->setConcurrencyLevel(concurrencyLevel);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }
    return this;

  _GF_MG_EXCEPTION_CATCH_ALL2
}

gc_ptr(RegionFactory) RegionFactory::SetLruEntriesLimit(System::UInt32 entriesLimit) {
  try {
    m_nativeptr->get()->setLruEntriesLimit(entriesLimit);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetDiskPolicy(DiskPolicyType diskPolicy) {
  try {
    m_nativeptr->get()->setDiskPolicy(static_cast<native::DiskPolicyType>(diskPolicy));
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetCachingEnabled(bool cachingEnabled) {
  try {
    m_nativeptr->get()->setCachingEnabled(cachingEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetCloningEnabled(bool cloningEnabled) {
  try {
    m_nativeptr->get()->setCloningEnabled(cloningEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

gc_ptr(RegionFactory) RegionFactory::SetConcurrencyChecksEnabled(bool concurrencyChecksEnabled) {
  try {
    m_nativeptr->get()->setConcurrencyChecksEnabled(concurrencyChecksEnabled);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}
// NEW GENERIC APIs:

GENERIC(class TKey, class TValue)
gc_ptr(IRegion<TKey, TValue>) RegionFactory::Create(gc_ptr(String) regionName) {
  _GF_MG_EXCEPTION_TRY2 /* due to auto replace */

    try {
      auto nativeptr = m_nativeptr->get()->create(marshal_as<std::string>(regionName));
      return Client::Region<TKey, TValue>::Create(nativeptr);
    } finally {
      GC::KeepAlive(m_nativeptr);
    }

  _GF_MG_EXCEPTION_CATCH_ALL2 /* due to auto replace */
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionFactory) RegionFactory::SetCacheLoader(gc_ptr(Client::ICacheLoader<TKey, TValue>) cacheLoader) {
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
gc_ptr(RegionFactory) RegionFactory::SetCacheWriter(gc_ptr(Client::ICacheWriter<TKey, TValue>) cacheWriter) {
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
gc_ptr(RegionFactory) RegionFactory::SetCacheListener(gc_ptr(Client::ICacheListener<TKey, TValue>) cacheListener) {
  std::shared_ptr<native::CacheListener> listenerptr;
  if (cacheListener != nullptr) {
    gc_ptr(CacheListenerGeneric<TKey, TValue>) clg = gcnew CacheListenerGeneric<TKey, TValue>();
    clg->SetCacheListener(cacheListener);
    listenerptr =
        std::shared_ptr<native::ManagedCacheListenerGeneric>(new native::ManagedCacheListenerGeneric(cacheListener));
    ((native::ManagedCacheListenerGeneric*)listenerptr.get())->setptr(clg);
    /*
    listenerptr = new native::ManagedCacheListenerGeneric(
      (gc_ptr(Client::ICacheListener<gc_ptr(Object), gc_ptr(Object)>))cacheListener);
      */
  }
  try {
    m_nativeptr->get()->setCacheListener(listenerptr);
  } finally {
    GC::KeepAlive(m_nativeptr);
  }
  return this;
}

GENERIC(class TKey, class TValue)
gc_ptr(RegionFactory) RegionFactory::SetPartitionResolver(gc_ptr(Client::IPartitionResolver<TKey, TValue>)
                                                              partitionresolver) {
  std::shared_ptr<native::PartitionResolver> resolverptr;
  if (partitionresolver != nullptr) {
    gc_ptr(Client::IFixedPartitionResolver<TKey, TValue>) resolver =
        dynamic_cast<gc_ptr(Client::IFixedPartitionResolver<TKey, TValue>)>(partitionresolver);
    if (resolver != nullptr) {
      gc_ptr(FixedPartitionResolverGeneric<TKey, TValue>) prg = gcnew FixedPartitionResolverGeneric<TKey, TValue>();
      prg->SetPartitionResolver(resolver);
      resolverptr = std::shared_ptr<native::ManagedFixedPartitionResolverGeneric>(
          new native::ManagedFixedPartitionResolverGeneric(resolver));
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
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
