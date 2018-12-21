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

#ifndef BENCHMARK_UTIL_HPP
#define BENCHMARK_UTIL_HPP

#include "util/string.hpp"

using apache::geode::client::to_utf16;
using apache::geode::client::to_utf8;

constexpr char32_t LATIN_CAPITAL_LETTER_C = U'\U00000043';
constexpr char32_t INVERTED_EXCLAMATION_MARK = U'\U000000A1';
constexpr char32_t SAMARITAN_PUNCTUATION_ZIQAA = U'\U00000838';
constexpr char32_t LINEAR_B_SYLLABLE_B008_A = U'\U00010000';

template <class ToString, class FromString>
ToString convert(const FromString& from);

template <>
inline std::string convert(const std::u32string& from) {
  return to_utf8(from);
}

template <>
inline std::u16string convert(const std::u32string& from) {
  return to_utf16(from);
}

#endif  // BENCHMARK_UTIL_HPP
