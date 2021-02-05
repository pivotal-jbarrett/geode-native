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

//#include "../../../ICacheWriter.hpp"
#include "../CacheWriterAdapter.hpp"
#include "../ICacheWriter.hpp"
//#include "../Region.hpp"
//#include "../../../Region.hpp"
//#include "../../../Cache.hpp"

using namespace System;

using namespace Apache::Geode::Client;

namespace Apache {
namespace Geode {
namespace Client {

GENERIC(class TKey, class TValue)
PUBLIC ref class CacheWriterGeneric : Apache::Geode::Client::CacheWriterAdapter<gc_ptr(Object), gc_ptr(Object)> {
 private:
  gc_ptr(ICacheWriter<TKey, TValue>) m_writer;

 public:
  void SetCacheWriter(gc_ptr(ICacheWriter<TKey, TValue>) writer) { m_writer = writer; }

  bool BeforeUpdate(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) ev) override {
    EntryEvent<TKey, TValue> gevent(ev->GetNative());
    return m_writer->BeforeUpdate(% gevent);
  }

  bool BeforeCreate(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) ev) override {
    EntryEvent<TKey, TValue> gevent(ev->GetNative());
    return m_writer->BeforeCreate(% gevent);
  }

  bool BeforeDestroy(gc_ptr(Apache::Geode::Client::EntryEvent<gc_ptr(Object), gc_ptr(Object)>) ev) override {
    EntryEvent<TKey, TValue> gevent(ev->GetNative());
    return m_writer->BeforeDestroy(% gevent);
  }

  bool BeforeRegionClear(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>) ev) override {
    RegionEvent<TKey, TValue> gevent(ev->GetNative());
    return m_writer->BeforeRegionClear(% gevent);
  }

  bool BeforeRegionDestroy(gc_ptr(Apache::Geode::Client::RegionEvent<gc_ptr(Object), gc_ptr(Object)>) ev) override {
    RegionEvent<TKey, TValue> gevent(ev->GetNative());
    return m_writer->BeforeRegionDestroy(% gevent);
  }

  void Close(gc_ptr(Apache::Geode::Client::IRegion<gc_ptr(Object), gc_ptr(Object)>) region) override {
    m_writer->Close((gc_ptr(IRegion<TKey, TValue>))region);
  }
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
