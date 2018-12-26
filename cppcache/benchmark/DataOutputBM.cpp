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

#include <benchmark/benchmark.h>

#include <limits>

#include <geode/DataOutput.hpp>

#include "DataOutputInternal.hpp"
#include "SerializationRegistry.hpp"
#include "util.hpp"

using apache::geode::client::Cache;
using apache::geode::client::CacheableString;
using apache::geode::client::DataOutput;
using apache::geode::client::DataOutputInternal;
using apache::geode::client::SerializationRegistry;

static void DataOutputBMWriteReset(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteReset);

static void DataOutputBMWriteEnsureCapacity(benchmark::State& state) {
  auto range = state.range(0);

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.ensureCapacity(range);
  }
}
BENCHMARK(DataOutputBMWriteEnsureCapacity)->Range(8, 8 << 14);

static void DataOutputBMWriteUint8(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.write(static_cast<uint8_t>(55U));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteUint8);

static void DataOutputBMWriteInt8(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.write(static_cast<int8_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteInt8);

static void DataOutputBMWriteInt16(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<int16_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteInt16);

static void DataOutputBMWriteUint16(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<uint16_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteUint16);

static void DataOutputBMWriteInt32(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<int32_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteInt32);

static void DataOutputBMWriteUint32(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<uint32_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteUint32);

static void DataOutputBMWriteInt64(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<int64_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteInt64);

static void DataOutputBMWriteUint64(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeInt(static_cast<uint64_t>(55));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteUint64);

static void DataOutputBMWriteBoolean(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeBoolean(true);
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteBoolean);

static void DataOutputBMWriteFloat(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeFloat(3.14f);
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteFloat);

static void DataOutputBMWriteDouble(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeDouble(3.14);
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteDouble);

static void DataOutputBMWriteArrayLen(benchmark::State& state) {
  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeArrayLen(static_cast<int32_t>(state.range(0)));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteArrayLen)
    ->Arg(-1)
    ->Arg(0)
    ->Arg(253)
    ->Arg(0xFFFF + 1);

static void DataOutputBMWriteBytesUnsigned(benchmark::State& state) {
  const auto bytes = std::vector<uint8_t>(state.range(0));

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeBytes(bytes.data(), static_cast<int32_t>(bytes.size()));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteBytesUnsigned)->Range(8, 8 << 10);

static void DataOutputBMWriteBytesSigned(benchmark::State& state) {
  const auto bytes = std::vector<int8_t>(state.range(0));

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeBytes(bytes.data(), static_cast<int32_t>(bytes.size()));
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteBytesSigned)->Range(8, 8 << 10);

static void DataOutputBMWriteBytesOnlyUnsigned(benchmark::State& state) {
  const auto bytes = std::vector<uint8_t>(state.range(0));

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeBytesOnly(bytes.data(), bytes.size());
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteBytesOnlyUnsigned)->Range(8, 8 << 10);

static void DataOutputBMWriteBytesOnlySigned(benchmark::State& state) {
  auto bytes = std::vector<int8_t>(state.range(0));

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeBytesOnly(bytes.data(), bytes.size());
    dataOutput.reset();
  }
}
BENCHMARK(DataOutputBMWriteBytesOnlySigned)->Range(8, 8 << 10);

template <class String, char32_t UnicodeChar>
void DataOutputBMWriteString(benchmark::State& state) {
  const std::u32string u32String(state.range(0), UnicodeChar);
  const String string = convert<String>(u32String);

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeString(string);
    dataOutput.reset();
  }
}

// Chooses different path if string JMUTF8 string could be > 64k
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::string, LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::u16string,
                   LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::string,
                   INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::u16string,
                   INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::u16string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::string,
                   LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 14);
BENCHMARK_TEMPLATE(DataOutputBMWriteString, std::u16string,
                   LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 14);

template <class String, char32_t UnicodeChar>
void DataOutputBMWriteUTF(benchmark::State& state) {
  const std::u32string u32String(state.range(0), UnicodeChar);
  const String string = convert<String>(u32String);

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeUTF(string);
    dataOutput.reset();
  }
}

BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::string, LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::u16string, LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::string, INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::u16string,
                   INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::u16string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::string, LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteUTF, std::u16string,
                   LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 10);

template <class String, char32_t UnicodeChar>
void DataOutputBMWriteChars(benchmark::State& state) {
  const std::u32string u32String(state.range(0), UnicodeChar);
  const String string = convert<String>(u32String);

  DataOutputInternal dataOutput;
  for (auto _ : state) {
    dataOutput.writeChars(string);
    dataOutput.reset();
  }
}

BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::string, LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::u16string,
                   LATIN_CAPITAL_LETTER_C)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::string,
                   INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::u16string,
                   INVERTED_EXCLAMATION_MARK)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::u16string,
                   SAMARITAN_PUNCTUATION_ZIQAA)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::string,
                   LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(DataOutputBMWriteChars, std::u16string,
                   LINEAR_B_SYLLABLE_B008_A)
    ->Range(8, 8 << 10);
