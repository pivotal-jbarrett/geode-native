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

#ifndef GEODE_HASH_H_
#define GEODE_HASH_H_

#include <chrono>
#include <string>

#include "internal/hash_impl.hpp"
#include "internal/type_traits.hpp"

namespace apache {
namespace geode {

/**
 * Hashes based on the same algorithm used in the Geode server.
 *
 * Based on \c std::hash.
 *
 * @tparam _T type to hash.
 */
template <typename _T, typename Enable>
struct hash {
  typedef _T argument_type;
  int32_t operator()(const argument_type& val) const;
};

/**
 * Hashes \c std::u16string like \c java.lang.String
 */
template <>
struct hash<std::u16string> {
  inline int32_t operator()(const std::u16string& val) const {
    int32_t hash = 0;
    for (auto&& c : val) {
      hash = 31 * hash + c;
    }
    return hash;
  }
};

/**
 * Hashes \c std::string like \c java.lang.String.
 */
template <>
struct hash<std::string> {
  inline int32_t operator()(const std::string& val) const {
    int32_t hash = 0;

    for (auto&& it = val.cbegin(); it < val.cend(); it++) {
      auto cp = static_cast<uint32_t>(0xff & *it);
      if (cp < 0x80) {
        // 1 byte
      } else if ((cp >> 5) == 0x6) {
        // 2 bytes
        ++it;
        cp = ((cp << 6) & 0x7ff) + ((*it) & 0x3f);
      } else if ((cp >> 4) == 0xe) {
        // 3 bytes
        ++it;
        cp = ((cp << 12) & 0xffff) + (((0xff & *it) << 6) & 0xfff);
        ++it;
        cp += (*it) & 0x3f;
      } else if ((cp >> 3) == 0x1e) {
        // 4 bytes
        ++it;
        cp = ((cp << 18) & 0x1fffff) + (((0xff & *it) << 12) & 0x3ffff);
        ++it;
        cp += ((0xff & *it) << 6) & 0xfff;
        ++it;
        cp += (*it) & 0x3f;
      } else {
        // ignore invalid character and attempt to recover
      }

      if (cp > 0xffff) {
        // surrogate pair
        hash = 31 * hash +
               static_cast<uint16_t>((cp >> 10) + (0xD800 - (0x10000 >> 10)));
        hash = 31 * hash + static_cast<uint16_t>((cp & 0x3ff) + 0xdc00u);
      } else {
        // single code unit
        hash = 31 * hash + cp;
      }
    }

    return hash;
  }
};

/**
 * Hashes like \c java.lang.Char (UTF-8)
 */
template <>
struct hash<char> {
  inline int32_t operator()(const char& val) const {
    return static_cast<int32_t>(val);
  }
};

/**
 * Hashes like \c java.lang.Char (UTF-16)
 */
template <>
struct hash<char16_t> {
  inline int32_t operator()(const int16_t& val) const {
    return static_cast<int32_t>(val);
  }
};

/**
 * Hashes like \c java.lang.Boolean
 */
template <>
struct hash<bool> {
  inline int32_t operator()(const bool& val) const { return val ? 1231 : 1237; }
};

/**
 * Hashes like \c java.lang.Byte
 */
template <>
struct hash<int8_t> {
  inline int32_t operator()(const int8_t& val) const {
    return static_cast<int32_t>(val);
  }
};

/**
 * Hashes like \c java.lang.Short
 */
template <>
struct hash<int16_t> {
  inline int32_t operator()(const int16_t& val) const {
    return static_cast<int32_t>(val);
  }
};

/**
 * Hashes like \c java.lang.Integer
 */
template <>
struct hash<int32_t> {
  inline int32_t operator()(const int32_t& val) const { return val; }
};

/**
 * Hashes like \c java.lang.Long
 */
template <>
struct hash<int64_t> {
  inline int32_t operator()(const int64_t& val) const {
    return static_cast<int32_t>(val ^ (val >> 32));
  }
};

/**
 * Hashes like \c java.lang.Float
 */
template <>
struct hash<float_t> {
  inline int32_t operator()(const float_t& val) const {
    union float_int32_t {
      float f;
      int32_t u;
    } v;
    v.f = isnan(val) ? std::numeric_limits<float_t>::quiet_NaN() : val;
    return v.u;
  }
};

/**
 * Hashes like \c java.lang.Double
 */
template <>
struct hash<double_t> {
  inline int32_t operator()(const double_t& val) const {
    union double_int64_t {
      double d;
      int64_t u;
    } v;
    v.d = isnan(val) ? std::numeric_limits<double_t>::quiet_NaN() : val;
    return hash<int64_t>{}(v.u);
  }
};

/**
 * Hashes like \c java.util.DateTime
 */
template <>
struct hash<std::chrono::system_clock::time_point> {
  inline int32_t operator()(
      const std::chrono::system_clock::time_point& val) const {
    return hash<int64_t>{}(
        std::chrono::duration_cast<std::chrono::milliseconds>(val -
                                                              internal::epoch)
            .count());
  }
};

/**
 * Hashes like \c java.lang.Object, by address.
 */
template <typename _T>
    struct hash<_T* /*, typename std::enable_if<std::is_pointer<_T>::value>::type*/>
{
  inline int32_t operator()(const _T* val) const {
    return hash<int64_t>{}(reinterpret_cast<int64_t>(val));
  }
};

/**
 * Hashes any type with \code int32_t hashcode() \endcode method.
 * @tparam _T type that implements hashcode method.
 */
template <typename _T>
struct hash<_T,
            typename std::enable_if<internal::has_hashcode<_T>::value>::type> {
  inline int32_t operator()(const _T& val) const { return val.hashcode(); }
};

/**
 * Hashes any type with int32_t hashcode() method.
 * @tparam _T type that implements hashcode method.
 */
template <typename _T>
struct hash<
    _T, typename std::enable_if<
            internal::is_smart_ptr<_T>::value &&
            internal::has_hashcode<typename _T::element_type>::value>::type> {
  inline int32_t operator()(const _T& val) const { return val->hashcode(); }
};

/*
template <typename _T>
int32_t hash_code(const _T& value);

template <typename _T>
struct hash<_T,
            typename std::enable_if<internal::has_hash_code<_T>::value>::type> {
  inline int32_t operator()(const _T& val) const { return hash_code(val); }
};
*/

/**
 * Hashes like \c java.util.Objects.hash(Object...)
 *
 * @tparam _Types value types.
 * @param values to hash.
 * @return hash of all values.
 */
template <typename... _Types>
inline int32_t hash_all(const _Types&... values) {
  return internal::hash_impl(1, values...);
}

}  // namespace geode
}  // namespace apache

#endif  // GEODE_UTIL_FUNCTIONAL_H_
