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

#include "ccache.h"

#include <geode/CacheFactory.hpp>

CacheFactory createCacheFactory() {
  CacheFactory cacheFactory = {new apache::geode::client::CacheFactory()};
  return cacheFactory;
}

void destroyCacheFactory(CacheFactory cacheFactory) {
  delete static_cast<apache::geode::client::CacheFactory*>(cacheFactory.ptr);
}

Cache createCache(CacheFactory cacheFactory) {
  Cache cache = {new apache::geode::client::Cache(
      static_cast<apache::geode::client::CacheFactory*>(cacheFactory.ptr)
          ->create())};
  return cache;
}

void destroyCache(Cache cache) {
  delete static_cast<apache::geode::client::Cache*>(cache.ptr);
}
