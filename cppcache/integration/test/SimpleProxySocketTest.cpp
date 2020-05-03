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
using ::testing::StrictMock;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

class MockACE_SOCK_Stream : public ACE_SOCK_Stream {
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

template <typename _Base =
              SimpleProxySocket<void, StrictMock<MockPlainSocket<>>>>
class MockSimpleProxySocket : public _Base {
 public:
  using base_type = _Base;
  using base_type::base_type;
  MockSimpleProxySocket(const std::string& hostname, uint16_t port)
      : base_type(hostname, port) {
    ON_CALL(*this, connect)
        .WillByDefault([&](const std::string& hostname, uint16_t port) {
          base_type::connect(hostname, port);
        });
  }
  MOCK_METHOD(void, connect, (const std::string&, uint16_t), (override));
  using base_type::getStream;
};

#pragma clang diagnostic pop

TEST(SimpleProxySocketTest, connect) {
  StrictMock<MockSimpleProxySocket<>> socket("proxy.invalid", 456);
  EXPECT_CALL(socket, connect("something.invalid", 123));
  EXPECT_CALL(dynamic_cast<decltype(socket)::base_type&>(socket),
              connect("proxy.invalid", 456));
  EXPECT_CALL(socket.getStream(), close());

  socket.connect("something.invalid", 123);
}

TEST(SimpleProxySocketTest, close) {
  StrictMock<MockSimpleProxySocket<>> socket("proxy.invalid", 456);

  EXPECT_CALL(socket, close());
  EXPECT_CALL(socket.getStream(), close());

  socket.close();
}

TEST(SimpleProxySocketTest, writeStringLiteral) {
  StrictMock<MockSimpleProxySocket<>> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();
  EXPECT_CALL(stream, send("foo", 4));
  EXPECT_CALL(stream, close());

  socket.write("foo");
}

TEST(SimpleProxySocketTest, writeStdString) {
  StrictMock<MockSimpleProxySocket<>> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();
  std::string str("foo");
  EXPECT_CALL(stream, send(str.data(), 3));
  EXPECT_CALL(stream, close());

  socket.write(str);
}

TEST(SimpleProxySocketTest, throwsExceptionOnFailedWrite) {
  StrictMock<MockSimpleProxySocket<>> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();
  EXPECT_CALL(stream, send(_, _)).WillRepeatedly(::testing::Return(-1));
  EXPECT_CALL(stream, close());

  EXPECT_THROW(socket.write("foo"), std::runtime_error);
}
