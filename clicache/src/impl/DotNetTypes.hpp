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

#include "../cli.hpp"

namespace Apache {
namespace Geode {
namespace Client {
namespace Internal {

CLI(PUBLIC ref class DotNetTypes sealed) {
 public:
  static gc_ptr(Type) IntType = Int32::typeid;
  static gc_ptr(Type) UIntType = UInt32::typeid;
  static gc_ptr(Type) StringType = String::typeid;
  static gc_ptr(Type) BooleanType = Boolean::typeid;
  static gc_ptr(Type) FloatType = Single::typeid;
  static gc_ptr(Type) DoubleType = Double::typeid;
  static gc_ptr(Type) CharType = Char::typeid;
  static gc_ptr(Type) SByteType = SByte::typeid;
  static gc_ptr(Type) ShortType = Int16::typeid;
  static gc_ptr(Type) UShortType = UInt16::typeid;
  static gc_ptr(Type) LongType = Int64::typeid;
  static gc_ptr(Type) ULongType = UInt64::typeid;
  static gc_ptr(Type) ByteArrayType = Type::GetType("System.Byte[]");
  static gc_ptr(Type) SByteArrayType = Type::GetType("System.SByte[]");
  static gc_ptr(Type) DoubleArrayType = Type::GetType("System.Double[]");
  static gc_ptr(Type) FloatArrayType = Type::GetType("System.Single[]");
  static gc_ptr(Type) ShortArrayType = Type::GetType("System.Int16[]");
  static gc_ptr(Type) UShortArrayType = Type::GetType("System.UInt16[]");
  static gc_ptr(Type) IntArrayType = Type::GetType("System.Int32[]");
  static gc_ptr(Type) UIntArrayType = Type::GetType("System.UInt32[]");
  static gc_ptr(Type) LongArrayType = Type::GetType("System.Int64[]");
  static gc_ptr(Type) ULongArrayType = Type::GetType("System.UInt64[]");
  static gc_ptr(Type) BoolArrayType = Type::GetType("System.Boolean[]");
  static gc_ptr(Type) CharArrayType = Type::GetType("System.Char[]");
  static gc_ptr(Type) StringArrayType = Type::GetType("System.String[]");
  static gc_ptr(Type) DateType = Type::GetType("System.DateTime");
  static gc_ptr(Type) ByteArrayOfArrayType = Type::GetType("System.Byte[][]");
  static gc_ptr(Type) ObjectArrayType = Type::GetType("System.Collections.Generic.List`1[System.Object]");

  static gc_ptr(Type) VoidType = Type::GetType("System.Void");
  static gc_ptr(Type) ObjectType = Type::GetType("System.Object");

  static gc_ptr(Type) GuidType = Guid::typeid;
  static gc_ptr(Type) DecimalType = Decimal::typeid;
};

}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
