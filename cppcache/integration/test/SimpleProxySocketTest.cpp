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

class SimpleProxySocketTest : public ::testing::Test {
 protected:
  SimpleProxySocketTest() {}

  ~SimpleProxySocketTest() override = default;

  void SetUp() override {}

  void TearDown() override{};
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

class MockACE_SOCK_Stream : public ACE_SOCK_Stream {
 public:
  MOCK_METHOD(int, send, (const void*, size_t));
  MOCK_METHOD(int, close, ());
};

class MockPlainSocket : public PlainSocket<StrictMock<MockACE_SOCK_Stream>> {
 public:
  using PlainSocket::PlainSocket;
  MOCK_METHOD(void, connect, (const std::string&, uint16_t), (override));
  MOCK_METHOD(void, close, (), (override));
  using PlainSocket::getStream;
};

class MockSimpleProxySocket : public SimpleProxySocket<void, MockPlainSocket> {
 public:
  using base_type = SimpleProxySocket<void, MockPlainSocket>;
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

TEST_F(SimpleProxySocketTest, connect) {
  StrictMock<MockSimpleProxySocket> socket("proxy.invalid", 456);
  EXPECT_CALL(socket, connect("something.invalid", 123));
  EXPECT_CALL(dynamic_cast<decltype(socket)::base_type&>(socket),
              connect("proxy.invalid", 456));

  socket.connect("something.invalid", 123);
}

TEST_F(SimpleProxySocketTest, close) {
  StrictMock<MockSimpleProxySocket> socket("proxy.invalid", 456);

  EXPECT_CALL(socket, close());

  socket.close();
}

TEST_F(SimpleProxySocketTest, writeStringLiteral) {
  StrictMock<MockSimpleProxySocket> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();

  EXPECT_CALL(stream, send("foo", 4));

  socket.write("foo");
}

TEST_F(SimpleProxySocketTest, writeStdString) {
  StrictMock<MockSimpleProxySocket> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();
  std::string str("foo");

  EXPECT_CALL(stream, send(str.data(), 3));

  socket.write(str);
}

TEST_F(SimpleProxySocketTest, throwsExceptionOnFailedWrite) {
  StrictMock<MockSimpleProxySocket> socket("proxy.invalid", 456);
  auto& stream = socket.getStream();

  EXPECT_CALL(stream, send(_, _)).WillRepeatedly(::testing::Return(-1));

  EXPECT_THROW(socket.write("foo"), std::runtime_error);
}
