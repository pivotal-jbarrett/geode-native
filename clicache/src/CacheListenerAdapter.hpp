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
#include "ICacheListener.hpp"

namespace Apache {
namespace Geode {
namespace Client {

/// <summary>
/// Utility class that implements all methods in <c>ICacheListener</c>
/// with empty implementations. Applications can subclass this class
/// and only override the methods for the events of interest.
/// </summary>
GENERIC(class TKey, class TValue)
PUBLIC ref class CacheListenerAdapter : public ICacheListener<TKey, TValue> {
 public:
  virtual void AfterCreate(gc_ptr(EntryEvent<TKey, TValue>) ev) {}

  virtual void AfterUpdate(gc_ptr(EntryEvent<TKey, TValue>) ev) {}

  virtual void AfterInvalidate(gc_ptr(EntryEvent<TKey, TValue>) ev) {}

  virtual void AfterDestroy(gc_ptr(EntryEvent<TKey, TValue>) ev) {}

  virtual void AfterRegionInvalidate(gc_ptr(RegionEvent<TKey, TValue>) ev) {}

  virtual void AfterRegionDestroy(gc_ptr(RegionEvent<TKey, TValue>) ev) {}

  virtual void AfterRegionLive(gc_ptr(RegionEvent<TKey, TValue>) ev) {}

  virtual void AfterRegionClear(gc_ptr(RegionEvent<TKey, TValue>) ev) {}

  virtual void Close(gc_ptr(Apache::Geode::Client::IRegion<TKey, TValue>) region) {}
  virtual void AfterRegionDisconnected(gc_ptr(Apache::Geode::Client::IRegion<TKey, TValue>) region) {}
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
