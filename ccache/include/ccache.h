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

#ifndef GEODE_CCACHE_H_
#define GEODE_CCACHE_H_

#include "ccache_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void* ptr;
} CacheFactory;

typedef struct {
  void* ptr;
} Cache;

CCACHE_EXPORT CacheFactory createCacheFactory(void);
CCACHE_EXPORT void destroyCacheFactory(CacheFactory cacheFactory);

CCACHE_EXPORT Cache createCache(CacheFactory cacheFactory);
CCACHE_EXPORT void destroyCache(Cache cache);

#ifdef __cplusplus
}
#endif

#endif  // GEODE_CCACHE_H_
