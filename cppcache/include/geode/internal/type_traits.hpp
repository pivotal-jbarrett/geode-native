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

#ifndef GEODE_INTERNAL_TYPE_TRAITS_H_
#define GEODE_INTERNAL_TYPE_TRAITS_H_

#include <codecvt>
#include <functional>
#include <locale>
#include <memory>
#include <string>
#include <type_traits>

namespace apache {
namespace geode {
namespace internal {

/**
 * Type trait that matches a member function <tt>int32_t hashcode()</tt>.
 * @tparam _T type to test
 */
template <typename _T, typename = void>
struct has_hashcode : std::false_type {};

/**
 * Type trait that matches a member function <tt>int32_t hashcode()</tt>.
 * @tparam _T type to test
 */
template <typename _T>
struct has_hashcode<
    _T, typename std::enable_if<std::is_same<
        decltype(std::declval<_T>().hashcode()), int32_t>::value>::type>
    : std::true_type {};

/**
 * Type trait that matches a member <tt>int32_t hash_code(const _T& val)</tt>.
 * @tparam _T type to test
 */
template <typename _T, typename = void>
struct has_hash_code : std::false_type {};

/**
 * Type trait that matches a member <tt>int32_t hash_code(const _T& val)</tt>.
 * @tparam _T type to test
 */
template <typename _T>
struct has_hash_code<
    _T, typename std::enable_if<std::is_same<
        decltype(hash_code(std::declval<_T>())), int32_t>::value>::type>
    : std::true_type {};


/**
 * Type trait that matches C++ std smart pointers.
 * @tparam _T type to test
 */
template <typename _T>
struct is_smart_ptr : std::false_type {};

/**
 * Matches \c std::shared_ptr.
 */
template <typename _T>
struct is_smart_ptr<std::shared_ptr<_T>> : std::true_type {};

/**
 * Matches \c std::weak_ptr.
 */
template <typename _T>
struct is_smart_ptr<std::weak_ptr<_T>> : std::true_type {};

/**
 * Matches \c std::unique_ptr.
 */
template <typename _T>
struct is_smart_ptr<std::unique_ptr<_T>> : std::true_type {};

}  // namespace internal
}  // namespace geode
}  // namespace apache

#endif  // GEODE_INTERNAL_TYPE_TRAITS_H_
