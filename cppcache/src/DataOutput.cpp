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

#include <vector>

#include <geode/DataOutput.hpp>
#include <geode/SystemProperties.hpp>

#include "CacheImpl.hpp"
#include "CacheRegionHelper.hpp"
#include "SerializationRegistry.hpp"
#include "util/JavaModifiedUtf8.hpp"
#include "util/Log.hpp"
#include "util/string.hpp"

namespace apache {
namespace geode {
namespace client {

std::recursive_mutex globalBigBufferMutex;
size_t DataOutput::m_highWaterMark = 50 * 1024 * 1024;
size_t DataOutput::m_lowWaterMark = 8192;

/** This represents a allocation in this thread local pool. */
class BufferDesc {
 public:
  uint8_t* m_buf;
  size_t m_size;

  BufferDesc(uint8_t* buf, size_t size) : m_buf(buf), m_size(size) {}

  BufferDesc() : m_buf(nullptr), m_size(0) {}

  ~BufferDesc() {}

  BufferDesc& operator=(const BufferDesc& other) {
    if (this != &other) {
      m_buf = other.m_buf;
      m_size = other.m_size;
    }
    return *this;
  }

  BufferDesc(const BufferDesc& other)
      : m_buf(other.m_buf), m_size(other.m_size) {}
};

/** Thread local pool of buffers for DataOutput objects. */
class TSSDataOutput {
 private:
  std::vector<BufferDesc> m_buffers;

 public:
  TSSDataOutput();
  ~TSSDataOutput();

  uint8_t* getBuffer(size_t* size) {
    if (!m_buffers.empty()) {
      BufferDesc desc = m_buffers.back();
      m_buffers.pop_back();
      *size = desc.m_size;
      return desc.m_buf;
    } else {
      uint8_t* buf;
      *size = 8192;
      buf = static_cast<uint8_t*>(std::malloc(8192 * sizeof(uint8_t)));
      if (buf == nullptr) {
        throw OutOfMemoryException("Out of Memory while resizing buffer");
      }
      return buf;
    }
  }

  void poolBuffer(uint8_t* buf, size_t size) {
    BufferDesc desc(buf, size);
    m_buffers.push_back(desc);
  }

  static thread_local TSSDataOutput threadLocalBufferPool;
};

TSSDataOutput::TSSDataOutput() : m_buffers() {
  m_buffers.reserve(10);
  LOGDEBUG("DATAOUTPUT poolsize is %d", m_buffers.size());
}

TSSDataOutput::~TSSDataOutput() {
  while (!m_buffers.empty()) {
    BufferDesc desc = m_buffers.back();
    m_buffers.pop_back();
    std::free(desc.m_buf);
  }
}

thread_local TSSDataOutput TSSDataOutput::threadLocalBufferPool;

DataOutput::DataOutput(const CacheImpl* cache, Pool* pool)
    : m_size(0), m_haveBigBuffer(false), m_cache(cache), m_pool(pool) {
  m_bytes.reset(DataOutput::checkoutBuffer(&m_size));
  m_buf = m_bytes.get();
}

uint8_t* DataOutput::checkoutBuffer(size_t* size) {
  return TSSDataOutput::threadLocalBufferPool.getBuffer(size);
}

void DataOutput::checkinBuffer(uint8_t* buffer, size_t size) {
  TSSDataOutput::threadLocalBufferPool.poolBuffer(buffer, size);
}

void DataOutput::writeObjectInternal(const std::shared_ptr<Serializable>& ptr,
                                     bool isDelta) {
  getSerializationRegistry().serialize(ptr, *this, isDelta);
}

void DataOutput::acquireLock() { globalBigBufferMutex.lock(); }

void DataOutput::releaseLock() { globalBigBufferMutex.unlock(); }

const SerializationRegistry& DataOutput::getSerializationRegistry() const {
  return *m_cache->getSerializationRegistry();
}

Cache* DataOutput::getCache() const { return m_cache->getCache(); }

template <class _Traits, class _Allocator>
void DataOutput::writeJavaModifiedUtf8(
    const std::basic_string<char, _Traits, _Allocator>& value) {
  // Save room for length, to be set at end
  writeInt(static_cast<uint16_t>(0));

  if (!value.empty()) {
    // Save room to come back later for length
    auto maxEncodedLength =
        static_cast<size_t>(std::numeric_limits<uint16_t>::max());

    size_t encodedLength = 0;

    to_utf16(value, [&](const char16_t* begin, const char16_t* end) {
      ensureCapacity((end - begin) * 3);
      for (; begin < end; begin++) {
        encodedLength += encodeJavaModifiedUtf8(*begin);
      }

      if (encodedLength >= maxEncodedLength) {
        encodedLength = maxEncodedLength;
        return false;
      }

      return true;
    });

    // Rewrite encoded length
    auto tmp = m_buf;
    m_buf -= sizeof(uint16_t) + encodedLength;
    writeInt(static_cast<uint16_t>(encodedLength));
    m_buf = tmp;
  }
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void
DataOutput::writeJavaModifiedUtf8(const std::string&);

template <class _Traits, class _Allocator>
void DataOutput::writeJavaModifiedUtf8(
    const std::basic_string<char32_t, _Traits, _Allocator>& value) {
  /*
   * OPTIMIZE convert from UCS-4 to CESU-8/Java Modified UTF-8 directly
   * http://www.unicode.org/reports/tr26/
   */
  if (value.empty()) {
    writeInt(static_cast<uint16_t>(0));
  } else {
    writeJavaModifiedUtf8(to_utf16(value));
  }
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void
DataOutput::writeJavaModifiedUtf8(const std::u32string&);

void DataOutput::writeJavaModifiedUtf8(const char32_t* data, size_t len) {
  // TODO string optimize from UCS-4 to jmutf8
  if (0 == len) {
    writeInt(static_cast<uint16_t>(0));
  } else {
    writeJavaModifiedUtf8(to_utf16(data, len));
  }
}

size_t DataOutput::getJavaModifiedUtf8EncodedLength(const char16_t* data,
                                                    size_t length) {
  return internal::JavaModifiedUtf8::encodedLength(data, length);
}

template <class _Traits, class _Allocator>
void DataOutput::writeUtf16Huge(
    const std::basic_string<char, _Traits, _Allocator>& value) {
  if (value.empty()) {
    writeInt(static_cast<uint16_t>(0));
  } else {
    writeUtf16Huge(value.data(), value.length());
  }
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void DataOutput::writeUtf16Huge(
    const std::string&);

void DataOutput::writeUtf16Huge(const char* utf8, size_t utf8Length) {
  size_t utf16Length = 0;

  // save space for length later;
  writeInt(static_cast<uint32_t>(utf16Length));

  utf16Length =
      writeUtf16(utf8, utf8Length, std::numeric_limits<uint32_t>::max());

  // Rewrite encoded length
  auto tmp = m_buf;
  m_buf -= sizeof(uint32_t) + (utf16Length * sizeof(char16_t));
  writeInt(static_cast<uint32_t>(utf16Length));
  m_buf = tmp;
}

template <class _Traits, class _Allocator>
void DataOutput::writeUtf16Huge(
    const std::basic_string<char32_t, _Traits, _Allocator>& value) {
  // TODO string OPTIMIZE convert from UCS-4 to UTF-16 directly
  if (value.empty()) {
    writeInt(static_cast<uint16_t>(0));
  } else {
    writeUtf16Huge(to_utf16(value));
  }
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void DataOutput::writeUtf16Huge(
    const std::u32string&);

void DataOutput::writeUtf16Huge(const char32_t* data, size_t len) {
  // TODO string optimize from UCS-4 to UTF-16
  if (0 == len) {
    writeInt(static_cast<uint16_t>(0));
  } else {
    writeUtf16Huge(to_utf16(data, len));
  }
}

template <class _Traits, class _Allocator>
void DataOutput::writeUtf16(
    const std::basic_string<char, _Traits, _Allocator>& value) {
  writeUtf16(value.data(), value.length());
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void DataOutput::writeUtf16(
    const std::string&);

size_t DataOutput::writeUtf16(const char* utf8, size_t utf8Length,
                              size_t maxUtf16Length) {
  size_t utf16Length = 0;

  to_utf16(utf8, utf8 + utf8Length,
           [&](const char16_t* begin, const char16_t* end) {
             auto length = end - begin;
             writeUtf16(begin, length);
             utf16Length += length;
             if (utf16Length >= maxUtf16Length) {
               utf16Length = maxUtf16Length;
               return false;
             }

             return true;
           });

  return utf16Length;
}

template <class _Traits, class _Allocator>
void DataOutput::writeUtf16(
    const std::basic_string<char32_t, _Traits, _Allocator>& value) {
  // TODO string OPTIMIZE convert from UCS-4 to UTF-16 directly
  if (!value.empty()) {
    writeUtf16(to_utf16(value));
  }
}
template APACHE_GEODE_EXPLICIT_TEMPLATE_EXPORT void DataOutput::writeUtf16(
    const std::u32string&);

void DataOutput::writeUtf16(const char32_t* data, size_t len) {
  // TODO string optimize from UCS-4 to UTF-16
  if (len > 0) {
    writeUtf16(to_utf16(data, len));
  }
}

}  // namespace client
}  // namespace geode
}  // namespace apache
