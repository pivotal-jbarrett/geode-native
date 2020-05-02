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

#include <thread>

#include <ace/OS_Errno.h>
#include <ace/SSL/SSL_SOCK_Connector.h>

#include <gtest/gtest.h>

class SniProxyTest : public ::testing::Test {
 protected:
  SniProxyTest() {}

  ~SniProxyTest() override = default;

  void SetUp() override {}

  void TearDown() override{};
};

#define ACE_LAST_ERROR ACE_OS::strerror(ACE_OS::last_error())

// class SniProxySockConnector {
// public:
//  explicit SniProxySockConnector(ACE_INET_Addr proxyAddress)
//      : proxyAddress_(proxyAddress) {}
//
//  SniProxySockConnector(ACE_SOCK_Stream &new_stream, const ACE_Addr
//  &remote_sap,
//                        const ACE_Time_Value *timeout = 0,
//                        const ACE_Addr &local_sap = ACE_Addr::sap_any,
//                        int reuse_addr = 0, int flags = 0, int perms = 0) {}
//
// private:
//  ACE_INET_Addr proxyAddress_;
//};

class Conn {
 protected:
  Conn() = default;
  virtual void writeVoid(const void* buffer, size_t length) = 0;

 public:
  Conn(const Conn&) = delete;
  virtual ~Conn() noexcept = default;

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
class TcpStream : public Conn {
 protected:
  _Stream stream_;

 public:
  void writeVoid(const void* buffer, size_t length) override {
    if (stream_.send(buffer, length) == -1) {
      throw std::runtime_error(ACE_LAST_ERROR);
    }
  }

  _Stream& getStream() { return stream_; }
};

class TcpConn : public TcpStream<ACE_SOCK_Stream> {
 private:
  void doClose() { stream_.close(); }

 protected:
  std::string hostname_;
  uint16_t port_;

  TcpConn(const std::string& hostname, uint16_t port)
      : hostname_(hostname), port_(port) {}

  void connect() {
    ACE_SOCK_Connector connector;
    ACE_INET_Addr address(port_, hostname_.c_str());
    if (connector.connect(stream_, address) == -1) {
      throw std::runtime_error(ACE_LAST_ERROR);
    }
  }

 public:
  TcpConn() = default;
  ~TcpConn() noexcept override = default;

  void connect(const std::string& hostname, uint16_t port) override {
    hostname_ = hostname;
    port_ = port;

    connect();
  }

  void close() override { doClose(); }
};

class ProxyTcpConn : public TcpConn {
 public:
  ProxyTcpConn(const std::string& hostname, uint16_t port)
      : TcpConn(hostname, port) {}
  ~ProxyTcpConn() noexcept override = default;

  void connect(const std::string&, uint16_t) override { TcpConn::connect(); }
};

class SslTcpConn : public TcpStream<ACE_SSL_SOCK_Stream> {
 private:
  std::unique_ptr<TcpConn> plainConnection_;

  void doClose() {
    stream_.close();
    plainConnection_->close();
  }

  TcpConn& getOrCreatePlainConnection() {
    if (!plainConnection_) {
      plainConnection_ = std::unique_ptr<TcpConn>(new TcpConn());
    }
    return *plainConnection_;
  }

 public:
  SslTcpConn() = default;
  explicit SslTcpConn(std::unique_ptr<TcpConn> plainConnection)
      : plainConnection_(std::move(plainConnection)) {}
  ~SslTcpConn() noexcept override { doClose(); }

  void connect(const std::string& hostname, uint16_t port) override {
    auto& plainConnection = getOrCreatePlainConnection();

    plainConnection.connect(hostname, port);

    stream_.set_handle(plainConnection.getStream().get_handle());
    SSL_set_tlsext_host_name(stream_.ssl(),
                             const_cast<char*>(hostname.c_str()));
    ACE_SSL_SOCK_Connector sslConnector;
    if (sslConnector.complete(stream_) == -1) {
      throw std::runtime_error(ACE_LAST_ERROR);
    }
  }

  void close() override { doClose(); }
};

TEST_F(SniProxyTest, test) {
  auto proxyTcpConn =
      std::unique_ptr<ProxyTcpConn>(new ProxyTcpConn("172.217.164.228", 443));

  auto sslConn =
      std::unique_ptr<SslTcpConn>(new SslTcpConn(std::move(proxyTcpConn)));

  sslConn->connect("thisservershouldnotexist.invalid", 123);
  sslConn->write("chars");
  sslConn->write(std::string("string"));
  sslConn->close();
}
