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

/**
 * \file
 *
 * Internals for multi-value hash_all function \c ::apache::geode::hash_all().
 */

#pragma once

#ifndef GEODE_INTERNAL_HASH_IMPL_H_
#define GEODE_INTERNAL_HASH_IMPL_H_

#include "../hash.hpp"

namespace apache {
namespace geode {

template <typename, typename = void>
struct hash;

namespace internal {

template <typename _Head>
inline int32_t hash_impl(int32_t hash, const _Head& head) {
  return hash * 31 + apache::geode::hash<_Head>{}(head);
}

template <typename _Head, typename... _Tail>
inline int32_t hash_impl(int32_t hash, const _Head& head,
                         const _Tail&... tail) {
  return hash_impl(hash_impl(hash, head), tail...);
}

static const auto epoch = std::chrono::system_clock::from_time_t(0);

}  // namespace internal
}  // namespace geode
}  // namespace apache

#endif  // GEODE_INTERNAL_HASH_H_
