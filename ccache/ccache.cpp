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

thread_local std::exception* current_exception = nullptr;

template <typename R>
R catchUnhandledException(std::function<R(void)> function) {
  try {
    return function();
  } catch (const apache::geode::client::Exception& e) {
    current_exception = new apache::geode::client::Exception(e);
  } catch (const std::exception& e) {
    current_exception = new apache::geode::client::Exception(e.what());
  }

  return R{};
}

geode_CacheFactory* geode_createCacheFactory() {
  return catchUnhandledException<geode_CacheFactory*>([]() {
    return reinterpret_cast<geode_CacheFactory*>(
        new apache::geode::client::CacheFactory());
  });
}

bool geode_destroyCacheFactory(geode_CacheFactory* cacheFactory) {
  return catchUnhandledException<bool>([&]() {
    delete reinterpret_cast<apache::geode::client::CacheFactory*>(cacheFactory);
    return true;
  });
}

geode_Cache* geode_CacheFactory_createCache(geode_CacheFactory* cacheFactory) {
  return catchUnhandledException<geode_Cache*>([&]() {
    return reinterpret_cast<geode_Cache*>(new apache::geode::client::Cache(
        reinterpret_cast<apache::geode::client::CacheFactory*>(cacheFactory)
            ->create()));
  });
}

bool geode_destroyCache(geode_Cache* cache) {
  return catchUnhandledException<bool>([&]() {
    delete reinterpret_cast<apache::geode::client::Cache*>(cache);
    return true;
  });
}

bool geode_Cache_throwsGeodeException(geode_Cache*) {
  return catchUnhandledException<bool>([]() -> bool {
    throw apache::geode::client::Exception("fake exception");
    //    return true;
  });
}

bool geode_Cache_throwsRuntimeException(geode_Cache*) {
  return catchUnhandledException<bool>([]() -> bool {
    throw std::runtime_error("fake runtime error");
    //    return true;
  });
}

bool geode_checkException(void) { return current_exception; }

const geode_Exception* geode_getException(void) {
  return reinterpret_cast<geode_Exception*>(current_exception);
}

void geode_clearException(void) {
  if (current_exception) {
    delete current_exception;
    current_exception = nullptr;
  }
}

const char* geode_Exception_getMessage(const geode_Exception* exception) {
  auto e = reinterpret_cast<const std::exception*>(exception);
  return e->what();
}

const char* geode_Exception_getStackTrace(const geode_Exception* exception) {
  auto e = reinterpret_cast<const std::exception*>(exception);
  if (auto g = dynamic_cast<const apache::geode::client::Exception*>(e)) {
    return g->getStackTrace().c_str();
  }
  return "";
}
