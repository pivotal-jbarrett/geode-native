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

//#include "../DataOutput.hpp"
#include "../begin_native.hpp"
#include <geode/DataOutput.hpp>
#include "SerializationRegistry.hpp"
#include "../end_native.hpp"

#include "../IPdxSerializable.hpp"
using namespace System;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;
ref class DataOutput;
ref class DataInput;
namespace Internal {
ref class PdxType;
PUBLIC ref class PdxHelper {
 public:
  static void SerializePdx(gc_ptr(DataOutput) dataOutput, gc_ptr(IPdxSerializable) pdxObject);

  static gc_ptr(IPdxSerializable) DeserializePdx(gc_ptr(DataInput) dataOutput, bool forceDeserialize,
                                                 const native::SerializationRegistry* serializationRegistry);

  static gc_ptr(IPdxSerializable) PdxHelper::DeserializePdx(gc_ptr(DataInput) dataInput, bool forceDeserialize,
                                                            int typeId, int length,
                                                            const native::SerializationRegistry* serializationRegistry);

  literal Byte PdxHeader = 8;

  static Int32 ReadInt32(System::Byte* offsetPosition);

  static Int32 ReadInt16(System::Byte* offsetPosition);

  static Int32 PdxHelper::ReadUInt16(System::Byte* offsetPosition);

  static Int32 ReadByte(System::Byte* offsetPosition);

  static void WriteInt32(System::Byte* offsetPosition, Int32 value);

  static void WriteInt16(System::Byte* offsetPosition, Int32 value);

  static void WriteByte(System::Byte* offsetPosition, Int32 value);

  static Int32 ReadInt(System::Byte* offsetPosition, int size);

  static Int32 GetEnumValue(gc_ptr(String) enumClassName, gc_ptr(String) enumName, int hashcode, gc_ptr(Cache) cache);

  static gc_ptr(Object) GetEnum(int enumId, gc_ptr(Cache) cache);

 private:
  static void CreateMergedType(gc_ptr(PdxType) localType, gc_ptr(PdxType) remoteType, gc_ptr(DataInput) dataInput,
                               const native::SerializationRegistry* serializationRegistry);
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
