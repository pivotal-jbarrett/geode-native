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

  void writeVoid(const void* buffer, size_t length) override {
    if (stream_.send(buffer, length) == -1) {
      throw std::runtime_error(GEODE_ACE_LAST_ERROR);
    }
  }

 public:
  _Stream& getStream() { return stream_; }
};

template <typename _Stream = ACE_SOCK_Stream>
class PlainSocket : public StreamSocket<_Stream> {
 protected:
  using base_type = StreamSocket<_Stream>;
  using base_type::stream_;

 public:
  using stream_type = _Stream;

  PlainSocket() = default;
  ~PlainSocket() noexcept override { doClose(); };

  void connect(const std::string& hostname, uint16_t port) override {
    ACE_SOCK_Connector connector;
    ACE_INET_Addr address(port, hostname.c_str());
    if (connector.connect(stream_, address) == -1) {
      throw std::runtime_error(GEODE_ACE_LAST_ERROR);
    }
  }

  void close() override { doClose(); }

 private:
  void doClose() { stream_.close(); }
};

template <typename _Stream = ACE_SOCK_Stream,
          typename _Base = PlainSocket<_Stream>>
class SimpleProxySocket : public _Base {
 private:
  std::string hostname_;
  uint16_t port_;

 public:
  SimpleProxySocket(const std::string& hostname, uint16_t port)
      : hostname_(hostname), port_(port) {}
  ~SimpleProxySocket() noexcept override = default;

  void connect(const std::string&, uint16_t) override {
    _Base::connect(hostname_, port_);
  }
};

template <typename _Stream = ACE_SSL_SOCK_Stream,
          typename _PlainSocket = PlainSocket<>,
          typename _PlainStream = typename _PlainSocket::stream_type>
class SslSocket : public StreamSocket<_Stream> {
 protected:
  using stream_type = _Stream;
  using base_type = StreamSocket<_Stream>;
  using plain_stream_type = StreamSocket<_PlainStream>;
  using base_type::stream_;

 private:
  std::unique_ptr<plain_stream_type> plainConnection_;

  void doClose() {
    stream_.close();
    if (plainConnection_) {
      plainConnection_->close();
    }
  }

  plain_stream_type& getOrCreatePlainConnection() {
    if (!plainConnection_) {
      plainConnection_ = std::unique_ptr<plain_stream_type>(new _PlainSocket());
    }
    return *plainConnection_;
  }

 public:
  SslSocket() = default;
  explicit SslSocket(std::unique_ptr<plain_stream_type> plainSocket)
      : plainConnection_(std::move(plainSocket)) {}
  ~SslSocket() noexcept override { doClose(); }

  void connect(const std::string& hostname, uint16_t port) override {
    auto& plainConnection = getOrCreatePlainConnection();

    plainConnection.connect(hostname, port);

    stream_.set_handle(plainConnection.getStream().get_handle());
    SSL_set_tlsext_host_name(stream_.ssl(),
                             const_cast<char*>(hostname.c_str()));
    complete();
  }

  void close() override { doClose(); }

 protected:
  virtual void complete() {
    ACE_SSL_SOCK_Connector sslConnector;
    if (sslConnector.complete(stream_) == -1) {
      throw std::runtime_error(GEODE_ACE_LAST_ERROR);
    }
  }

  virtual SSL* getSsl() { return stream_.ssl(); }
};
#endif  // GEODE_SOCKET_H
