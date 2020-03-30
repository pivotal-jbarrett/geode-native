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

#include <ccache.h>
#include <stdio.h>

int main(void) {
  geode_CacheFactory* cacheFactory = geode_createCacheFactory();
  if (cacheFactory) {
    geode_Cache* cache = geode_CacheFactory_createCache(cacheFactory);
    if (cache) {
      if (!geode_Cache_throwsGeodeException(cache)) {
        const geode_Exception* exception = geode_getException();
        printf("Exception calling Cache::throwsException: %s\n%s\n",
               geode_Exception_getMessage(exception),
               geode_Exception_getStackTrace(exception));
        geode_clearException();
      }

      if (!geode_Cache_throwsRuntimeException(cache)) {
        const geode_Exception* exception = geode_getException();
        printf("Exception calling Cache::throwsException: %s\n%s\n",
               geode_Exception_getMessage(exception),
               geode_Exception_getStackTrace(exception));
        geode_clearException();
      }

      geode_destroyCacheFactory(cacheFactory);
      geode_destroyCache(cache);
    } else {
      const geode_Exception* exception = geode_getException();
      printf("Exception CacheFactory.createCache(): %s\n%s\n",
             geode_Exception_getMessage(exception),
             geode_Exception_getStackTrace(exception));
      geode_clearException();
    }
  } else {
    const geode_Exception* exception = geode_getException();
    printf("Exception creating CacheFactory: %s\n%s\n",
           geode_Exception_getMessage(exception),
           geode_Exception_getStackTrace(exception));
    geode_clearException();
  }
}
