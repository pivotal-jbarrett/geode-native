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

#include <gmock/gmock.h>

#include <gtest/gtest.h>

#include "Socket.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::StrictMock;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

class MockACE_SOCK_Stream : public ACE_SOCK_Stream {
 public:
  MOCK_METHOD(int, send, (const void*, size_t));
  MOCK_METHOD(int, close, ());
};

class MockACE_SSL_SOCK_Stream : public ACE_SSL_SOCK_Stream {
 public:
  MOCK_METHOD(int, send, (const void*, size_t));
  MOCK_METHOD(int, close, ());
};

template <typename _Base = PlainSocket<StrictMock<MockACE_SOCK_Stream>>>
class MockPlainSocket : public _Base {
 public:
  using base_type = _Base;
  using base_type::base_type;
  MOCK_METHOD(void, connect, (const std::string&, uint16_t), (override));
  MOCK_METHOD(void, close, (), (override));
  using base_type::getStream;
};

template <typename _Base = SslSocket<StrictMock<MockACE_SSL_SOCK_Stream>,
                                     StrictMock<MockPlainSocket<>>>>
class MockSslSocket : public _Base {
 public:
  using base_type = _Base;
  using base_type::base_type;
  MOCK_METHOD(void, complete, (), (override));
  using base_type::getSsl;
  using base_type::getStream;
};

#pragma clang diagnostic pop

TEST(SslSocketTest, connect) {
  auto plainSocket = std::unique_ptr<StrictMock<MockPlainSocket<>>>(
      new StrictMock<MockPlainSocket<>>());
  EXPECT_CALL(*plainSocket, connect("ssl.invalid", 123));
  EXPECT_CALL(*plainSocket, close());
  EXPECT_CALL(plainSocket->getStream(), close());

  StrictMock<MockSslSocket<>> sslSocket(std::move(plainSocket));
  EXPECT_CALL(sslSocket, complete());
  EXPECT_CALL(sslSocket.getStream(), close());

  sslSocket.connect("ssl.invalid", 123);
}

TEST(SslSocketTest, connectSetSni) {
  auto plainSocket = std::unique_ptr<StrictMock<MockPlainSocket<>>>(
      new StrictMock<MockPlainSocket<>>());
  EXPECT_CALL(*plainSocket, connect(_, _));
  EXPECT_CALL(*plainSocket, close());
  EXPECT_CALL(plainSocket->getStream(), close());

  StrictMock<MockSslSocket<>> sslSocket(std::move(plainSocket));
  EXPECT_CALL(sslSocket, complete());
  EXPECT_CALL(sslSocket.getStream(), close());

  sslSocket.connect("ssl.invalid", 123);
  auto ssl = sslSocket.getSsl();
  EXPECT_THAT(SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name),
              ::testing::StrEq("ssl.invalid"));
}

TEST(SslSocketTest, close) {
  auto plainSocket = std::unique_ptr<StrictMock<MockPlainSocket<>>>(
      new StrictMock<MockPlainSocket<>>());
  EXPECT_CALL(*plainSocket, close()).Times(2);
  EXPECT_CALL(plainSocket->getStream(), close());

  StrictMock<MockSslSocket<>> sslSocket(std::move(plainSocket));
  EXPECT_CALL(sslSocket.getStream(), close()).Times(2);

  sslSocket.close();
}

TEST(SslSocketTest, writeStringLiteral) {
  auto plainSocket = std::unique_ptr<PlainSocket<>>(new PlainSocket<>());

  SslSocket<StrictMock<MockACE_SSL_SOCK_Stream>> sslSocket(
      std::move(plainSocket));
  auto& sslStream = sslSocket.getStream();
  EXPECT_CALL(sslStream, close());
  EXPECT_CALL(sslStream, send("foo", 4));

  sslSocket.write("foo");
}

TEST(SslSocketTest, writeStdString) {
  const std::string str("foo");

  auto plainSocket = std::unique_ptr<PlainSocket<>>(new PlainSocket<>());

  SslSocket<StrictMock<MockACE_SSL_SOCK_Stream>> sslSocket(
      std::move(plainSocket));
  auto& sslStream = sslSocket.getStream();
  EXPECT_CALL(sslStream, close());
  EXPECT_CALL(sslStream, send(str.data(), 3));

  sslSocket.write(str);
}

TEST(SslSocketTest, throwsExceptionOnFailedWrite) {
  auto plainSocket = std::unique_ptr<PlainSocket<>>(new PlainSocket<>());

  SslSocket<StrictMock<MockACE_SSL_SOCK_Stream>> sslSocket(
      std::move(plainSocket));
  auto& sslStream = sslSocket.getStream();
  EXPECT_CALL(sslStream, close());
  EXPECT_CALL(sslStream, send(_, _)).WillRepeatedly(Return(-1));

  EXPECT_THROW(sslSocket.write("foo"), std::runtime_error);
}
