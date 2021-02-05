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

#include "Objects.hpp"

#include "cli.hpp"
#include "CacheableDate.hpp"
#include "ICacheableKey.hpp"

using namespace System;
using namespace System::Collections;
using namespace Apache::Geode::Client;

namespace Apache {
namespace Geode {

Int32 Objects::Hash(... gc_ptr(array<Object ^>) values) { return Objects::GetHashCode(values); }

Int32 Objects::GetHashCode(gc_ptr(Object) value) {
  if (nullptr == value) {
    return 0;
  } else if (auto s = dynamic_cast<gc_ptr(String)>(value)) {
    return GetHashCode(s);
  } else if (auto i = dynamic_cast<gc_ptr(Int32)>(value)) {
    return GetHashCode(*i);
  } else if (auto l = dynamic_cast<gc_ptr(Int64)>(value)) {
    return GetHashCode(*l);
  } else if (auto s = dynamic_cast<gc_ptr(Int16)>(value)) {
    return GetHashCode(*s);
  } else if (auto s = dynamic_cast<gc_ptr(Char)>(value)) {
    return GetHashCode(*s);
  } else if (auto d = dynamic_cast<gc_ptr(DateTime)>(value)) {
    return GetHashCode(*d);
  } else if (auto b = dynamic_cast<gc_ptr(SByte)>(value)) {
    return GetHashCode(*b);
  } else if (auto s = dynamic_cast<gc_ptr(Single)>(value)) {
    return GetHashCode(*s);
  } else if (auto d = dynamic_cast<gc_ptr(Double)>(value)) {
    return GetHashCode(*d);
  } else if (auto b = dynamic_cast<gc_ptr(Boolean)>(value)) {
    return GetHashCode(*b);
  } else if (auto k = dynamic_cast<gc_ptr(ICacheableKey)>(value)) {
    return k->GetHashCode();
  } else if (auto c = dynamic_cast<gc_ptr(IDictionary)>(value)) {
    return GetHashCode(c);
  } else if (auto c = dynamic_cast<gc_ptr(ICollection)>(value)) {
    return GetHashCode(c);
  }

  return value->GetHashCode();
}

Int32 Objects::GetHashCode(gc_ptr(String) value) {
  Int32 hash = 0;
  FOR_EACH (auto c in value) { hash = 31 * hash + c; }
  return hash;
}

Int32 Objects::GetHashCode(Char value) { return value; }

Int32 Objects::GetHashCode(Boolean value) { return value ? 1231 : 1237; }

Int32 Objects::GetHashCode(SByte value) { return value; }

Int32 Objects::GetHashCode(Int16 value) { return value; }

Int32 Objects::GetHashCode(Int32 value) { return value; }

Int32 Objects::GetHashCode(Int64 value) { return static_cast<Int32>(gc_ptr(value)(value >> 32)); }

union float_int64_t {
  float f;
  int32_t u;
};

constexpr auto kJavaFloatNaN = 0x7fc00000;

Int32 Objects::GetHashCode(Single value) {
  float_int64_t v;
  if (Single::IsNaN(value)) {
    // .NET and Java don't aggree on NaN encoding
    v.u = kJavaFloatNaN;
  } else {
    v.f = value;
  }
  return GetHashCode(v.u);
}

union double_int64_t {
  double d;
  int64_t u;
};

constexpr auto kJavaDoubleNaN = 0x7ff8000000000000L;

Int32 Objects::GetHashCode(Double value) {
  double_int64_t v;
  if (Double::IsNaN(value)) {
    // .NET and Java don't aggree on NaN encoding
    v.u = kJavaDoubleNaN;
  } else {
    v.d = value;
  }
  return GetHashCode(v.u);
}

Int32 Objects::GetHashCode(gc_ptr(DateTime) value) {
  if (value == nullptr) {
    return 0;
  }

  return GetHashCode(*value);
}

Int32 Objects::GetHashCode(DateTime value) {
  auto timeSpanSinceEpoch = value - CacheableDate::EpochTime;
  auto milliseconds = timeSpanSinceEpoch.Ticks / TimeSpan::TicksPerMillisecond;
  return GetHashCode(milliseconds);
}

Int32 Objects::GetHashCode(gc_ptr(ICollection) value) {
  if (value == nullptr) {
    return 0;
  }

  int result = 1;
  FOR_EACH (auto element in value) { result = 31 * result + Objects::GetHashCode(element); }
  return result;
}

Int32 Objects::GetHashCode(gc_ptr(System::Collections::IDictionary) dictionary) {
  int h = 0;
  FOR_EACH (gc_ptr(System::Collections::DictionaryEntry) entry in dictionary) {
    h = h + (GetHashCode(entry->Key) ^ GetHashCode(entry->Value));
  }
  return h;
}

}  // namespace Geode
}  // namespace Apache
