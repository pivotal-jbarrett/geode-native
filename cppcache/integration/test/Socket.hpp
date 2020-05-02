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

#ifndef GEODE_SOCKET_H
#define GEODE_SOCKET_H

#include <ace/OS_Errno.h>
#include <ace/SSL/SSL_SOCK_Connector.h>

#define GEODE_ACE_LAST_ERROR ACE_OS::strerror(ACE_OS::last_error())

class Socket {
 protected:
  Socket() = default;
  virtual void writeVoid(const void* buffer, size_t length) = 0;

 public:
  Socket(const Socket&) = delete;
  virtual ~Socket() noexcept = default;

  virtual void connect(const std::string& hostname, uint16_t port) = 0;
  virtual void close() = 0;

  template <typename _Buffer>
  void write(const _Buffer& buffer) {
    writeVoid(buffer.data(), buffer.size());
  }

  template <size_t N>
  void write(const char (&buffer)[N]) {
    writeVoid(buffer, N);
  }
};

template <typename _Stream>
class StreamSocket : public Socket {
 protected:
  _Stream stream_;

 protected:
  void writeVoid(const void* buffer, size_t length) override {
    if (stream_.send(buffer, length) == -1) {
      throw std::runtime_error(GEODE_ACE_LAST_ERROR);
    }
  }

  _Stream& getStream() { return stream_; }
};

template <typename _Stream = ACE_SOCK_Stream>
class PlainSocket : public StreamSocket<_Stream> {
 private:
  using StreamSocket<_Stream>::stream_;
  void doClose() { stream_.close(); }

 protected:
  std::string hostname_;
  uint16_t port_;

  PlainSocket(const std::string& hostname, uint16_t port)
      : hostname_(hostname), port_(port) {}

  void connect() {
    ACE_SOCK_Connector connector;
    ACE_INET_Addr address(port_, hostname_.c_str());
    if (connector.connect(stream_, address) == -1) {
      throw std::runtime_error(GEODE_ACE_LAST_ERROR);
    }
  }

 public:
  PlainSocket() = default;
  ~PlainSocket() noexcept override = default;

  void connect(const std::string& hostname, uint16_t port) override {
    hostname_ = hostname;
    port_ = port;

    connect();
  }

  void close() override { doClose(); }
};

template <typename _Stream = ACE_SOCK_Stream>
class SimpleProxySocket : public PlainSocket<_Stream> {
 protected:
  using PlainSocket<_Stream>::connect;

 public:
  SimpleProxySocket(const std::string& hostname, uint16_t port)
      : PlainSocket<_Stream>(hostname, port) {}
  ~SimpleProxySocket() noexcept override = default;

  void connect(const std::string&, uint16_t) override { connect(); }
};

#endif  // GEODE_SOCKET_H
