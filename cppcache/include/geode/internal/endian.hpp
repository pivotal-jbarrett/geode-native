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

#ifndef GEODE_INTERNAL_ENDIAN_H_
#define GEODE_INTERNAL_ENDIAN_H_

#include "type_traits.hpp"

namespace apache {
namespace geode {
namespace client {
namespace internal {
namespace endian_convert {

inline uint16_t native_to_big(uint16_t native) {
  if (endian::native == endian::little) {
    return ((((native) >> 8) & 0xff) | (((native)&0xff) << 8));
  } else {
    return native;
  }
}

inline uint32_t native_to_big(uint32_t native) {
  if (endian::native == endian::little) {
    return ((((native)&0xff000000) >> 24) | (((native)&0x00ff0000) >> 8) |
            (((native)&0x0000ff00) << 8) | (((native)&0x000000ff) << 24));
  } else {
    return native;
  }
}

inline uint64_t native_to_big(uint64_t native) {
  if (endian::native == endian::little) {
    return ((((native)&0xff00000000000000ull) >> 56) |
            (((native)&0x00ff000000000000ull) >> 40) |
            (((native)&0x0000ff0000000000ull) >> 24) |
            (((native)&0x000000ff00000000ull) >> 8) |
            (((native)&0x00000000ff000000ull) << 8) |
            (((native)&0x0000000000ff0000ull) << 24) |
            (((native)&0x000000000000ff00ull) << 40) |
            (((native)&0x00000000000000ffull) << 56));
  } else {
    return native;
  }
}

}  // namespace endian_convert
}  // namespace internal
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_INTERNAL_ENDIAN_H_
