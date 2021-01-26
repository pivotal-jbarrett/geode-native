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

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

public
ref class Objects {
 public:
  static Int32 Hash(... array<Object^>^ values);
  static Int32 GetHashCode(array<Object^>^ value);
  static Int32 GetHashCode(Object^ value);
  static Int32 GetHashCode(String^ value);
  static Int32 GetHashCode(Char value);
  static Int32 GetHashCode(Boolean value);
  static Int32 GetHashCode(SByte value);
  static Int32 GetHashCode(Int16 value);
  static Int32 GetHashCode(Int32 value);
  static Int32 GetHashCode(Int64 value);
  static Int32 GetHashCode(Single value);
  static Int32 GetHashCode(Double value);
  static Int32 GetHashCode(DateTime^ value);
  static Int32 GetHashCode(DateTime value);
};

}  // namespace Client
}  // namespace Geode
}  // namespace Apache
