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

#include <boost/algorithm/string.hpp>
#include <boost/asio/ip/host_name.hpp>

#include <gtest/gtest.h>

#include "Utils.hpp"

using apache::geode::client::Utils;

TEST(UtilsTest, getCanonicalHostnameWithEmptyHostname) {
  EXPECT_EQ("", Utils::getCanonicalHostname(""));
}

TEST(UtilsTest, getCanonicalHostnameWithLocalhost) {
  EXPECT_EQ(boost::asio::ip::host_name(),
            Utils::getCanonicalHostname("localhost"));
}

TEST(UtilsTest, getCanonicalHostnameWithLocalHostname) {
  const auto localFqdn = boost::asio::ip::host_name();
  const auto localHostname = localFqdn.substr(0, localFqdn.find("."));

  EXPECT_EQ(localHostname, Utils::getCanonicalHostname(localHostname));
}

TEST(UtilsTest, getCanonicalHostnameWithNxdomain) {
  EXPECT_EQ("nxdomain.test", Utils::getCanonicalHostname("nxdomain.test"));
}

TEST(UtilsTest, DISABLED_getCanonicalHostnameWithPtrRecord) {
  // Needs a hostname that does have a PTR associated with its A record.
  // These addresses are not stable for unit testing.
  EXPECT_EQ("sea15s08-in-f4.1e100.net",
            Utils::getCanonicalHostname("www.google.com"));
}

TEST(UtilsTest, DISABLED_getCanonicalHostnameWithoutPtrRecord) {
  // Needs a hostname that doesn't have a PTR associated with its A record.
  // These addresses are not stable for unit testing.
  EXPECT_EQ("151.101.65.69", Utils::getCanonicalHostname("serverfault.com"));
}

TEST(UtilsTest, convertHostToCanonicalForm) {
  EXPECT_EQ("", Utils::convertHostToCanonicalForm("noport.test"));

  EXPECT_EQ("withport.test:123",
            Utils::convertHostToCanonicalForm("withport.test:123"));
}
