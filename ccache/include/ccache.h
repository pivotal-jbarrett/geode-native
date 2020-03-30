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

#include <stdbool.h>
#include <stddef.h>

#include "ccache_export.h"

#ifdef __cplusplus
extern "C" {
#endif

struct geode_CacheFactory;
typedef struct geode_CacheFactory geode_CacheFactory;

struct geode_Cache;
typedef struct geode_Cache geode_Cache;

struct geode_Exception;
typedef struct geode_Exception geode_Exception;

struct geode_StackTrace;
typedef struct geode_StackTrace geode_StackTrace;

CCACHE_EXPORT geode_CacheFactory* geode_createCacheFactory(void);
CCACHE_EXPORT bool geode_destroyCacheFactory(geode_CacheFactory* cacheFactory);

CCACHE_EXPORT geode_Cache* geode_CacheFactory_createCache(
    geode_CacheFactory* cacheFactory);
CCACHE_EXPORT bool geode_destroyCache(geode_Cache* cache);

CCACHE_EXPORT bool geode_Cache_throwsGeodeException(geode_Cache* cache);
CCACHE_EXPORT bool geode_Cache_throwsRuntimeException(geode_Cache* cache);

CCACHE_EXPORT bool geode_checkException(void);
CCACHE_EXPORT const geode_Exception* geode_getException(void);
CCACHE_EXPORT void geode_clearException(void);

CCACHE_EXPORT const char* geode_Exception_getMessage(
    const geode_Exception* exception);
CCACHE_EXPORT const char* geode_Exception_getStackTrace(
    const geode_Exception* exception);

#ifdef __cplusplus
}
#endif

#endif  // GEODE_CCACHE_H_
