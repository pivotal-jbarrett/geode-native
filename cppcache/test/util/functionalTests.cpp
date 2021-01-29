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

#include <limits>
#include <string>

#include <gtest/gtest.h>

#include <geode/internal/functional.hpp>

using apache::geode::client::internal::geode_hash;

TEST(geode_hash, bool) {
  constexpr auto hash = geode_hash<bool>{};

  EXPECT_EQ(1237, hash(false));
  EXPECT_EQ(1231, hash(true));
}

TEST(geode_hash, int8_t) {
  constexpr auto hash = geode_hash<int8_t>{};

  EXPECT_EQ(0, hash(0));
  EXPECT_EQ(1, hash(1));
  EXPECT_EQ(-1, hash(-1));
  EXPECT_EQ(std::numeric_limits<int8_t>::max(),
            hash(std::numeric_limits<int8_t>::max()));
  EXPECT_EQ(std::numeric_limits<int8_t>::min(),
            hash(std::numeric_limits<int8_t>::min()));
}

TEST(geode_hash, int16_t) {
  constexpr auto hash = geode_hash<int16_t>{};

  EXPECT_EQ(0, hash(0));
  EXPECT_EQ(1, hash(1));
  EXPECT_EQ(-1, hash(-1));
  EXPECT_EQ(std::numeric_limits<int16_t>::max(),
            hash(std::numeric_limits<int16_t>::max()));
  EXPECT_EQ(std::numeric_limits<int16_t>::min(),
            hash(std::numeric_limits<int16_t>::min()));
}

TEST(geode_hash, int32_t) {
  constexpr auto hash = geode_hash<int32_t>{};

  EXPECT_EQ(0, hash(0));
  EXPECT_EQ(1, hash(1));
  EXPECT_EQ(-1, hash(-1));
  EXPECT_EQ(std::numeric_limits<int32_t>::max(),
            hash(std::numeric_limits<int32_t>::max()));
  EXPECT_EQ(std::numeric_limits<int32_t>::min(),
            hash(std::numeric_limits<int32_t>::min()));
}

TEST(geode_hash, int64_t) {
  constexpr auto int32_max = std::numeric_limits<int32_t>::max();
  constexpr auto int32_min = std::numeric_limits<int32_t>::min();
  constexpr auto int64_max = std::numeric_limits<int64_t>::max();
  constexpr auto int64_min = std::numeric_limits<int64_t>::min();
  constexpr auto hash = geode_hash<int64_t>{};

  EXPECT_EQ(0, hash(0));
  EXPECT_EQ(1, hash(1));
  EXPECT_EQ(0, hash(-1));
  EXPECT_EQ(2147483647, hash(int32_max));
  EXPECT_EQ(2147483647, hash(int32_min));
  EXPECT_EQ(-2147483648, hash(1l + int32_max));
  EXPECT_EQ(-2147483648, hash(-1l + int32_min));
  EXPECT_EQ(-2147483648, hash(int64_max));
  EXPECT_EQ(-2147483648, hash(int64_min));
  EXPECT_EQ(-1073741824, hash(int64_max >> 1));
  EXPECT_EQ(-1073741824, hash(int64_min >> 1));
  EXPECT_EQ(-536870912, hash(int64_max >> 2));
  EXPECT_EQ(-536870912, hash(int64_min >> 2));
  EXPECT_EQ(0, hash(-9223372034707292160l));

  EXPECT_EQ(1583802735, hash(1577836800000L));
}

TEST(geode_hash, float_t) {
  constexpr auto hash = geode_hash<float_t>{};

  EXPECT_EQ(0, hash(0.0f));
  EXPECT_EQ(-2147483648, hash(-0.0f));
  EXPECT_EQ(1065353216, hash(1.0f));
  EXPECT_EQ(-1082130432, hash(-1.0f));
  EXPECT_EQ(2139095039, hash(std::numeric_limits<float_t>::max()));
  EXPECT_EQ(-8388609, hash(std::numeric_limits<float_t>::lowest()));
  EXPECT_EQ(1, hash(std::numeric_limits<float_t>::denorm_min()));
  EXPECT_EQ(2139095040, hash(std::numeric_limits<float_t>::infinity()));
  EXPECT_EQ(-8388608, hash(-1.0f * std::numeric_limits<float_t>::infinity()));
  EXPECT_EQ(8388608, hash(std::numeric_limits<float_t>::min()));
  EXPECT_EQ(2143289344, hash(std::numeric_limits<float_t>::quiet_NaN()));
  EXPECT_EQ(2143289344, hash(std::numeric_limits<float_t>::signaling_NaN()));
}

TEST(geode_hash, double_t) {
  constexpr auto hash = geode_hash<double_t>{};

  EXPECT_EQ(0, hash(0.0f));
  EXPECT_EQ(-2147483648, hash(-0.0));
  EXPECT_EQ(1072693248, hash(1.0));
  EXPECT_EQ(-1074790400, hash(-1.0));
  EXPECT_EQ(-2146435072, hash(std::numeric_limits<double_t>::max()));
  EXPECT_EQ(1048576, hash(std::numeric_limits<double_t>::lowest()));
  EXPECT_EQ(1, hash(std::numeric_limits<double_t>::denorm_min()));
  EXPECT_EQ(2146435072, hash(std::numeric_limits<double_t>::infinity()));
  EXPECT_EQ(-1048576, hash(-1.0f * std::numeric_limits<double_t>::infinity()));
  EXPECT_EQ(1048576, hash(std::numeric_limits<double_t>::min()));
  EXPECT_EQ(2146959360, hash(std::numeric_limits<double_t>::quiet_NaN()));
  EXPECT_EQ(2146959360, hash(std::numeric_limits<double_t>::signaling_NaN()));
}

TEST(geode_hash, string) {
  constexpr auto hash = geode_hash<std::string>{};

  EXPECT_EQ(0, hash(""));
  EXPECT_EQ(48, hash("0"));
  EXPECT_EQ(57, hash("9"));
  EXPECT_EQ(97, hash("a"));
  EXPECT_EQ(122, hash("z"));
  EXPECT_EQ(65, hash("A"));
  EXPECT_EQ(90, hash("Z"));

  EXPECT_EQ(1077910243, hash("supercalifragilisticexpialidocious"));

  EXPECT_EQ(1544552287, hash(u8"You had me at meat tornad\u00F6!\U000F0000"));

  auto str = std::string("You had me at");
  str.push_back(0);
  str.append(u8"meat tornad\u00F6!\U000F0000");
  EXPECT_EQ(701776767, hash(str));

  EXPECT_EQ(
      512895612,
      hash("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
           "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
           "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
           "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
           "reprehenderit in voluptate velit esse cillum dolore eu fugiat "
           "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
           "sunt in culpa qui officia deserunt mollit anim id est laborum."));
  EXPECT_EQ(
      -1425027716,
      hash(u8"\u16bb\u16d6\u0020\u16b3\u16b9\u16ab\u16a6\u0020\u16a6\u16ab"
           "\u16cf\u0020\u16bb\u16d6\u0020\u16d2\u16a2\u16de\u16d6\u0020"
           "\u16a9\u16be\u0020\u16a6\u16ab\u16d7\u0020\u16da\u16aa\u16be"
           "\u16de\u16d6\u0020\u16be\u16a9\u16b1\u16a6\u16b9\u16d6\u16aa"
           "\u16b1\u16de\u16a2\u16d7\u0020\u16b9\u16c1\u16a6\u0020\u16a6"
           "\u16aa\u0020\u16b9\u16d6\u16e5\u16ab"));
}

TEST(geode_hash, char) {
  constexpr auto hash = geode_hash<char>{};

  EXPECT_EQ(48, hash('0'));
  EXPECT_EQ(57, hash('9'));
  EXPECT_EQ(97, hash('a'));
  EXPECT_EQ(122, hash('z'));
  EXPECT_EQ(65, hash('A'));
  EXPECT_EQ(90, hash('Z'));
}

TEST(geode_hash, char16_t) {
  constexpr auto hash = geode_hash<char16_t>{};

  EXPECT_EQ(48, hash(u'0'));
  EXPECT_EQ(57, hash(u'9'));
  EXPECT_EQ(97, hash(u'a'));
  EXPECT_EQ(122, hash(u'z'));
  EXPECT_EQ(65, hash(u'A'));
  EXPECT_EQ(90, hash(u'Z'));
  EXPECT_EQ(5819, hash(u'\u16bb'));
}

TEST(geode_hash, timepoint) {
  using std::chrono::system_clock;
  constexpr auto hash = geode_hash<system_clock::time_point>{};

  EXPECT_EQ(0, hash(system_clock::from_time_t(0)));
  EXPECT_EQ(1583802735, hash(system_clock::from_time_t(1577836800)));
  EXPECT_EQ(-927080926, hash(system_clock::from_time_t(33134745600)));
  EXPECT_EQ(1670202000, hash(system_clock::from_time_t(-1577923200)));
  EXPECT_EQ(542840753, hash(system_clock::from_time_t(-4733596800)));
}

TEST(hash, 1int32_t) {
  using apache::geode::client::internal::hash;

  EXPECT_EQ(31, hash(0));
  EXPECT_EQ(32, hash(1));
  EXPECT_EQ(-2147483618, hash(std::numeric_limits<int32_t>::max()));
  EXPECT_EQ(-2147483617, hash(std::numeric_limits<int32_t>::min()));
}

TEST(hash, 2int32_t) {
  using apache::geode::client::internal::hash;

  EXPECT_EQ(962, hash(0, 1));
  EXPECT_EQ(992, hash(1, 0));
  EXPECT_EQ(930, hash(std::numeric_limits<int32_t>::max(),
                      std::numeric_limits<int32_t>::min()));
  EXPECT_EQ(960, hash(std::numeric_limits<int32_t>::min(),
                      std::numeric_limits<int32_t>::max()));
}

TEST(hash, 4int32_t) {
  using apache::geode::client::internal::hash;

  EXPECT_EQ(924547, hash(0, 1, 2, 3));
  EXPECT_EQ(953279, hash(1, 0, -1, -2));
}

TEST(hash, mulitpleTypes) {
  using apache::geode::client::internal::hash;

  EXPECT_EQ(-1009437857, hash(std::chrono::system_clock::from_time_t(0), true,
                              std::numeric_limits<int8_t>::max(),
                              std::numeric_limits<int16_t>::min(),
                              std::numeric_limits<int32_t>::max(),
                              std::numeric_limits<int64_t>::min(),
                              std::numeric_limits<float>::infinity(),
                              std::numeric_limits<double>::max(), 'C',
                              std::string("a string")));
}

#include <geode/CacheableString.hpp>

TEST(geode_hash, CacheableString) {
  using apache::geode::client::CacheableString;

  auto value = apache::geode::client::CacheableString::create(
      "supercalifragilisticexpialidocious");

  EXPECT_EQ(1077910243, geode_hash<decltype(*value)>{}(*value));
  EXPECT_EQ(1077910243, geode_hash<decltype(value)>{}(value));
}

TEST(hash, CacheableString) {
  using apache::geode::client::internal::hash;

  auto value = apache::geode::client::CacheableString::create(
      "supercalifragilisticexpialidocious");

  EXPECT_EQ(1077910274, hash(*value));
  EXPECT_EQ(1077910274, hash(value));
}

class CustomKey : apache::geode::client::CacheableKey {
 private:
  int32_t a_;
  double b_;
  std::string c_;

 public:
  CustomKey(int32_t a, double b, std::string c)
      : a_(a), b_(b), c_(std::move(c)) {}
  ~CustomKey() noexcept override = default;
  bool operator==(const CacheableKey&) const override { return false; }
  int32_t hashcode() const override {
    return apache::geode::client::internal::hash(a_, b_, c_);
  }
};

TEST(geode_hash, CustomKey) {
  auto value = std::make_shared<CustomKey>(1, 2.0, "key");
  EXPECT_EQ(-1073604993, geode_hash<decltype(value)>{}(value));
}
