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

class MockStream : public ACE_SOCK_Stream {
 public:
  MOCK_METHOD(int, send, (const void*, size_t));
  MOCK_METHOD(int, close, ());
};

class MockSocket : public PlainSocket<StrictMock<MockStream>> {
 public:
  MOCK_METHOD(void, connect, (const std::string&, uint16_t), (override));
  MOCK_METHOD(void, close, (), (override));
  using PlainSocket::getStream;
};

#pragma clang diagnostic pop

TEST(PlainSocketTest, connect) {
  StrictMock<MockSocket> socket;

  EXPECT_CALL(socket, connect("something.invalid", 123));

  socket.connect("something.invalid", 123);
}

TEST(PlainSocketTest, close) {
  StrictMock<MockSocket> socket;

  EXPECT_CALL(socket, close());

  socket.close();
}

TEST(PlainSocketTest, writeStringLiteral) {
  StrictMock<MockSocket> socket;
  auto& stream = socket.getStream();

  EXPECT_CALL(stream, send("foo", 4));

  socket.write("foo");
}

TEST(PlainSocketTest, writeStdString) {
  StrictMock<MockSocket> socket;
  auto& stream = socket.getStream();
  std::string str("foo");

  EXPECT_CALL(stream, send(str.data(), 3));

  socket.write(str);
}

TEST(PlainSocketTest, throwsExceptionOnFailedWrite) {
  StrictMock<MockSocket> socket;
  auto& stream = socket.getStream();

  EXPECT_CALL(stream, send(_, _)).WillRepeatedly(::testing::Return(-1));

  EXPECT_THROW(socket.write("foo"), std::runtime_error);
}
