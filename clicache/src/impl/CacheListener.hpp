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

#include "../ICacheListener.hpp"
#include "../CacheListenerAdapter.hpp"
#include "../ICacheListener.hpp"
#include "../Region.hpp"
//#include "../../../Region.hpp"
//#include "../../../Cache.hpp"

using namespace System;

using namespace Apache::Geode::Client;

namespace Apache {
namespace Geode {
namespace Client {

GENERIC(class TKey, class TValue)
PUBLIC ref class CacheListenerGeneric : CacheListenerAdapter<gc_ptr(Object), gc_ptr(Object)> {
 private:
  gc_ptr(ICacheListener<TKey, TValue>) m_listener;

 public:
  void SetCacheListener(gc_ptr(ICacheListener<TKey, TValue>) listener) { m_listener = listener; }

  virtual void AfterUpdate(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) event) override {
    EntryEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterUpdate(% gevent);
  }

  virtual void AfterCreate(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) event) override {
    EntryEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterCreate(% gevent);
  }

  virtual void AfterInvalidate(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>)
                                   event) override {
    EntryEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterInvalidate(% gevent);
  }

  virtual void AfterDestroy(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) event) override {
    EntryEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterDestroy(% gevent);
  }

  virtual void AfterRegionLive(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>)
                                   event) override {
    RegionEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterRegionLive(% gevent);
  }

  virtual void AfterRegionClear(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>)
                                    event) override {
    RegionEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterRegionClear(% gevent);
  }

  virtual void AfterRegionDestroy(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>)
                                      event) override {
    RegionEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterRegionDestroy(% gevent);
  }

  virtual void AfterRegionInvalidate(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>)
                                         event) override {
    RegionEvent<TKey, TValue> gevent(event->GetNative());
    m_listener->AfterRegionInvalidate(% gevent);
  }

  virtual void AfterRegionDisconnected(gc_ptr(Apache::Geode::Client::IRegion<gc_ptr(Object), gc_ptr(Object)>)
                                           region) override {
    auto gregion = Region<TKey, TValue>::Create(((gc_ptr(Region<gc_ptr(Object), gc_ptr(Object)>))region)->GetNative());
    m_listener->AfterRegionDisconnected(gregion);
  }

  virtual void Close(gc_ptr(Apache::Geode::Client::IRegion<gc_ptr(Object), gc_ptr(Object)>) region) override {
    auto gregion = Region<TKey, TValue>::Create(((gc_ptr(Region<gc_ptr(Object), gc_ptr(Object)>))region)->GetNative());
    m_listener->Close(gregion);
  }
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
