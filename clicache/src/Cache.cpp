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

#include "Cache.hpp"

#include "begin_native.hpp"
#include <CacheRegionHelper.hpp>
#include <CacheImpl.hpp>
#include "end_native.hpp"

#include "cli.hpp"

#include "ExceptionTypes.hpp"
#include "DistributedSystem.hpp"
#include "PoolFactory.hpp"
#include "Region.hpp"
#include "RegionAttributes.hpp"
#include "QueryService.hpp"
#include "CacheFactory.hpp"
#include "impl/AuthenticatedView.hpp"
#include "impl/SafeConvert.hpp"
#include "impl/PdxTypeRegistry.hpp"
#include "impl/PdxInstanceFactoryImpl.hpp"
#include "CacheTransactionManager.hpp"
#include "PoolManager.hpp"
#include "TypeRegistry.hpp"

#pragma warning(disable : 4091)

using namespace System;
using namespace msclr::interop;

namespace Apache {
namespace Geode {
namespace Client {

namespace native = apache::geode::client;

Cache::Cache(std::shared_ptr<native::Cache> nativeptr) {
  m_nativeptr = gcnew native_shared_ptr<native::Cache>(nativeptr);
  m_pdxTypeRegistry = gcnew Apache::Geode::Client::Internal::PdxTypeRegistry(this);
  m_typeRegistry = gcnew Apache::Geode::Client::TypeRegistry(this);
}

gc_ptr(String) Cache::Name::get() {
  BEGIN_NATIVE { return marshal_as<gc_ptr(String)>(m_nativeptr->get()->getName()); }
  END_NATIVE;
}

bool Cache::IsClosed::get() {
  BEGIN_NATIVE { return m_nativeptr->get()->isClosed(); }
  END_NATIVE;
}

gc_ptr(SystemProperties) Cache::SystemProperties::get() {
  BEGIN_NATIVE {
    auto &&systemProperties = m_nativeptr->get()->getSystemProperties();
    return Apache::Geode::Client::SystemProperties::Create(&systemProperties);
  }
  END_NATIVE;
}

gc_ptr(CacheTransactionManager) Cache::CacheTransactionManager::get() {
  // TODO shared_ptr this should be checking the return type for which tx mgr
  BEGIN_NATIVE {
    auto nativeptr = m_nativeptr->get()->getCacheTransactionManager();
    return Apache::Geode::Client::CacheTransactionManager::Create(nativeptr.get());
  }
  END_NATIVE;
}

void Cache::Close() { Close(false); }

void Cache::Close(bool keepalive) {
  try {
    BEGIN_NATIVE {
      m_nativeptr->get()->close(keepalive);
      Apache::Geode::Client::DistributedSystem::UnregisterBuiltinManagedTypes(this);
      CacheRegionHelper::getCacheImpl(m_nativeptr->get())->getPdxTypeRegistry()->clear();
    }
    END_NATIVE;
  } finally {
    m_typeRegistry->Clear();
    Apache::Geode::Client::DistributedSystem::unregisterCliCallback();
  }
}

void Cache::ReadyForEvents() {
  BEGIN_NATIVE { m_nativeptr->get()->readyForEvents(); }
  END_NATIVE;
}

GENERIC(class TKey, class TValue)
gc_ptr(Client::IRegion<TKey, TValue>) Cache::GetRegion(gc_ptr(String) path) {
  BEGIN_NATIVE {
    return Client::Region<TKey, TValue>::Create(m_nativeptr->get()->getRegion(marshal_as<std::string>(path)));
  }
  END_NATIVE;
}

GENERIC(class TKey, class TValue)
array<gc_ptr(Client::IRegion<TKey, TValue>)> ^ Cache::RootRegions() {
  std::vector<std::shared_ptr<apache::geode::client::Region>> vrr;
  BEGIN_NATIVE { vrr = m_nativeptr->get()->rootRegions(); }
  END_NATIVE;

  auto rootRegions = gcnew array<gc_ptr(Client::IRegion<TKey, TValue>)>(static_cast<int>(vrr.size()));

  for (System::Int32 index = 0; index < vrr.size(); index++) {
    std::shared_ptr<apache::geode::client::Region> &nativeptr(vrr[index]);
    rootRegions[index] = Client::Region<TKey, TValue>::Create(nativeptr);
  }
  return rootRegions;
}

gc_ptr(Client::QueryService) Cache::GetQueryService() {
  BEGIN_NATIVE { return Client::QueryService::Create(m_nativeptr->get()->getQueryService()); }
  END_NATIVE;
}

gc_ptr(Client::QueryService) Cache::GetQueryService(gc_ptr(String) poolName) {
  BEGIN_NATIVE { return QueryService::Create(m_nativeptr->get()->getQueryService(marshal_as<std::string>(poolName))); }
  END_NATIVE;
}

gc_ptr(RegionFactory) Cache::CreateRegionFactory(RegionShortcut preDefinedRegionAttributes) {
  auto preDefineRegionAttr = apache::geode::client::RegionShortcut(preDefinedRegionAttributes);

  BEGIN_NATIVE {
    return RegionFactory::Create(std::unique_ptr<native::RegionFactory>(
        new native::RegionFactory(m_nativeptr->get()->native::Cache::createRegionFactory(preDefineRegionAttr))));
  }
  END_NATIVE;
}

gc_ptr(IRegionService) Cache::CreateAuthenticatedView(gc_ptr(Properties<gc_ptr(String), gc_ptr(Object)>) credentials) {
  BEGIN_NATIVE {
    return AuthenticatedView::Create(
        m_nativeptr->get()->native::Cache::createAuthenticatedView(credentials->GetNative(), ""));
  }
  END_NATIVE;
}

bool Cache::GetPdxIgnoreUnreadFields() {
  BEGIN_NATIVE { return m_nativeptr->get()->native::Cache::getPdxIgnoreUnreadFields(); }
  END_NATIVE;
}

bool Cache::GetPdxReadSerialized() {
  BEGIN_NATIVE { return m_nativeptr->get()->native::Cache::getPdxReadSerialized(); }
  END_NATIVE;
}

gc_ptr(IRegionService) Cache::CreateAuthenticatedView(gc_ptr(Properties<gc_ptr(String), gc_ptr(Object)>) credentials,
                                                      gc_ptr(String) poolName) {
  BEGIN_NATIVE {
    return AuthenticatedView::Create(m_nativeptr->get()->native::Cache::createAuthenticatedView(
        credentials->GetNative(), marshal_as<std::string>(poolName)));
  }
  END_NATIVE;
}

void Cache::InitializeDeclarativeCache(gc_ptr(String) cacheXml) {
  BEGIN_NATIVE { m_nativeptr->get()->native::Cache::initializeDeclarativeCache(marshal_as<std::string>(cacheXml)); }
  END_NATIVE;
}

gc_ptr(IPdxInstanceFactory) Cache::CreatePdxInstanceFactory(gc_ptr(String) className) {
  return gcnew Internal::PdxInstanceFactoryImpl(className, this);
}

gc_ptr(DataInput) Cache::CreateDataInput(gc_ptr(array<Byte>) buffer, System::Int32 len) {
  return gcnew DataInput(buffer, len, this);
}

gc_ptr(DataInput) Cache::CreateDataInput(gc_ptr(array<Byte>) buffer) { return gcnew DataInput(buffer, this); }

gc_ptr(DataOutput) Cache::CreateDataOutput() { return gcnew DataOutput(this); }

gc_ptr(PoolFactory) Cache::GetPoolFactory() {
  BEGIN_NATIVE {
    return PoolFactory::Create(std::unique_ptr<native::PoolFactory>(
        new native::PoolFactory(m_nativeptr->get_shared_ptr()->getPoolManager().createFactory())));
  }
  END_NATIVE;
}

gc_ptr(PoolManager) Cache::GetPoolManager() {
  BEGIN_NATIVE { return gcnew PoolManager(m_nativeptr->get_shared_ptr()->getPoolManager()); }
  END_NATIVE;
}

}  // namespace Client
}  // namespace Geode
}  // namespace Apache
