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

#include "PdxLocalWriter.hpp"
#include "PdxHelper.hpp"
#include "PdxTypeRegistry.hpp"
#include "../DataOutput.hpp"
#include "DotNetTypes.hpp"
#include "../Cache.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {

PdxLocalWriter::PdxLocalWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(PdxType) pdxType) {
  m_dataOutput = dataOutput;
  m_pdxType = pdxType;
  m_currentOffsetIndex = 0;
  m_preserveData = nullptr;
  if (pdxType != nullptr) m_pdxClassName = pdxType->PdxClassName;
  // m_pdxDomainType = nullptr;
  initialize();
}

PdxLocalWriter::PdxLocalWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(PdxType) pdxType, gc_ptr(String) pdxClassName) {
  m_dataOutput = dataOutput;
  m_pdxType = pdxType;
  m_currentOffsetIndex = 0;
  m_preserveData = nullptr;
  // m_pdxDomainType = pdxDomainType;
  m_pdxClassName = pdxClassName;
  initialize();
}

void PdxLocalWriter::initialize() {
  if (m_pdxType != nullptr) {
    m_currentOffsetIndex = 0;
    // TODO:: 1 offset below if you can avoid
    m_offsets = gcnew array<int>(m_pdxType->NumberOfVarLenFields);
  }

  // start position, this should start of c++ dataoutput buffer and then use bufferlen
  m_startPosition = m_dataOutput->GetStartBufferPosition();
  // TODO: need to use this carefully
  m_startPositionOffset = static_cast<int>(m_dataOutput->BufferLength);  // data has been write
  m_dataOutput->AdvanceCursor(PdxHelper::PdxHeader);                     // to write pdx header
}

void PdxLocalWriter::AddOffset() {
  // bufferlength gives lenght which has been writeen to unmanged Dataoutput
  // m_startPositionOffset: from where pdx header length starts
  int bufferLen = static_cast<int>(m_dataOutput->BufferLength - m_startPositionOffset);
  int offset = bufferLen - PdxHelper::PdxHeader /* this needs to subtract*/;

  m_offsets[m_currentOffsetIndex++] = offset;
}

void PdxLocalWriter::EndObjectWriting() {
  // write header
  WritePdxHeader();
}

System::Byte* PdxLocalWriter::GetPdxStream(int& pdxLen) {
  System::Byte* stPos = m_dataOutput->GetStartBufferPosition() + m_startPositionOffset;
  int len = PdxHelper::ReadInt32(stPos);
  pdxLen = len;
  // ignore len and typeid
  return m_dataOutput->GetBytes(stPos + 8, len);
}

void PdxLocalWriter::WritePdxHeader() {
  Int32 len = calculateLenWithOffsets();
  Int32 typeId = m_pdxType->TypeId;

  // GetStartBufferPosition ; if unmanaged dataoutput get change
  System::Byte* starpos = m_dataOutput->GetStartBufferPosition() + m_startPositionOffset;
  PdxHelper::WriteInt32(starpos, len);
  PdxHelper::WriteInt32(starpos + 4, typeId);

  WriteOffsets(len);
}

Int32 PdxLocalWriter::calculateLenWithOffsets() {
  // int bufferLen = m_dataOutput->GetCursorPdx();
  int bufferLen = static_cast<int>(m_dataOutput->BufferLength - m_startPositionOffset);
  Int32 totalOffsets = 0;
  if (m_offsets->Length > 0) totalOffsets = m_offsets->Length - 1;  // for first var len no need to append offset
  Int32 totalLen = bufferLen - PdxHelper::PdxHeader + totalOffsets;

  if (totalLen <= 0xff)  // 1 byte
    return totalLen;
  else if (totalLen + totalOffsets <= 0xffff)  // 2 byte
    return totalLen + totalOffsets;
  else  // 4 byte
    return totalLen + totalOffsets * 3;
}

void PdxLocalWriter::WriteOffsets(Int32 len) {
  if (len <= 0xff) {
    for (int i = m_offsets->Length - 1; i > 0; i--) m_dataOutput->WriteByte((Byte)m_offsets[i]);

  } else if (len <= 0xffff) {
    for (int i = m_offsets->Length - 1; i > 0; i--) m_dataOutput->WriteUInt16((UInt16)m_offsets[i]);
  } else {
    for (int i = m_offsets->Length - 1; i > 0; i--) m_dataOutput->WriteUInt32((UInt32)m_offsets[i]);
  }
}

bool PdxLocalWriter::isFieldWritingStarted() { return true; }

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUnreadFields(gc_ptr(IPdxUnreadFields) unread) {
  if (isFieldWritingStarted()) {
    throw gcnew IllegalStateException("WriteUnreadFields must be called before " + "any other fields are written.");
  }

  if (unread != nullptr) {
    m_preserveData = (gc_ptr(PdxRemotePreservedData))unread;

    m_pdxType = m_dataOutput->Cache->GetPdxTypeRegistry()->GetPdxType(m_preserveData->MergedTypeId);
    if (m_pdxType == nullptr) {  // its local type
      // this needs to fix for IPdxTypemapper
      m_pdxType = GetLocalPdxType(m_pdxClassName);
    }
    m_offsets = gcnew array<int>(m_pdxType->NumberOfVarLenFields);
  }
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteByte(gc_ptr(String) fieldName, SByte value) {
  m_dataOutput->WriteSByte(value);
  return this;
}

void PdxLocalWriter::WriteByte(Byte byte) { m_dataOutput->WriteByte(byte); }

gc_ptr(IPdxWriter) PdxLocalWriter::WriteSByte(gc_ptr(String) fieldName, SByte value) {
  m_dataOutput->WriteSByte(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteBoolean(gc_ptr(String) fieldName, bool value) {
  m_dataOutput->WriteBoolean(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteChar(gc_ptr(String) fieldName, Char value) {
  m_dataOutput->WriteChar(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUInt16(gc_ptr(String) fieldName, System::UInt16 value) {
  m_dataOutput->WriteUInt16(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUInt32(gc_ptr(String) fieldName, System::UInt32 value) {
  m_dataOutput->WriteUInt32(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUInt64(gc_ptr(String) fieldName, System::UInt64 value) {
  m_dataOutput->WriteUInt64(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteShort(gc_ptr(String) fieldName, System::Int16 value) {
  m_dataOutput->WriteInt16(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteInt(gc_ptr(String) fieldName, System::Int32 value) {
  m_dataOutput->WriteInt32(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteLong(gc_ptr(String) fieldName, Int64 value) {
  m_dataOutput->WriteInt64(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteFloat(gc_ptr(String) fieldName, float value) {
  m_dataOutput->WriteFloat(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteDouble(gc_ptr(String) fieldName, double value) {
  m_dataOutput->WriteDouble(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteString(gc_ptr(String) fieldName, gc_ptr(String) value) {
  AddOffset();
  m_dataOutput->WriteString(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUTFHuge(gc_ptr(String) fieldName, gc_ptr(String) value) {
  AddOffset();
  m_dataOutput->WriteUTFHuge(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteASCIIHuge(gc_ptr(String) fieldName, gc_ptr(String) value) {
  AddOffset();
  m_dataOutput->WriteASCIIHuge(value);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteObject(gc_ptr(String) fieldName, gc_ptr(Object) obj) {
  AddOffset();
  m_dataOutput->WriteObject(obj);
  return this;
}

// TODO:
// gc_ptr(IPdxWriter) PdxLocalWriter::WriteMap( gc_ptr(String) fieldName,
// gc_ptr(System::Collections::IDictionary) map );

gc_ptr(IPdxWriter) PdxLocalWriter::WriteCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) obj) {
  AddOffset();
  m_dataOutput->WriteCollection(obj);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteDate(gc_ptr(String) fieldName, System::DateTime date) {
  m_dataOutput->WriteDate(date);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteBooleanArray(gc_ptr(String) fieldName, gc_ptr(array<bool>) boolArray) {
  AddOffset();
  m_dataOutput->WriteBooleanArray(boolArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteCharArray(gc_ptr(String) fieldName, gc_ptr(array<Char>) charArray) {
  AddOffset();
  m_dataOutput->WriteCharArray(charArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteByteArray(gc_ptr(String) fieldName, gc_ptr(array<Byte>) byteArray) {
  AddOffset();
  m_dataOutput->WriteBytes(byteArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteSByteArray(gc_ptr(String) fieldName, gc_ptr(array<SByte>) sbyteArray) {
  AddOffset();
  m_dataOutput->WriteSBytes(sbyteArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteShortArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int16>) shortArray) {
  AddOffset();
  m_dataOutput->WriteShortArray(shortArray);  // TODO::this don't write typeid looks confusing
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUnsignedShortArray(gc_ptr(String) fieldName,
                                                           gc_ptr(array<System::UInt16>) ushortArray) {
  AddOffset();
  // m_dataOutput->WriteObject(ushortArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int32>) intArray) {
  AddOffset();
  m_dataOutput->WriteIntArray(intArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUnsignedIntArray(gc_ptr(String) fieldName,
                                                         gc_ptr(array<System::UInt32>) uintArray) {
  AddOffset();
  // m_dataOutput->WriteObject(uintArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteLongArray(gc_ptr(String) fieldName, gc_ptr(array<Int64>) longArray) {
  AddOffset();
  m_dataOutput->WriteLongArray(longArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteUnsignedLongArray(gc_ptr(String) fieldName,
                                                          gc_ptr(array<System::UInt64>) ulongArray) {
  AddOffset();
  // m_dataOutput->WriteObject(ulongArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteFloatArray(gc_ptr(String) fieldName, gc_ptr(array<float>) floatArray) {
  AddOffset();
  m_dataOutput->WriteFloatArray(floatArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteDoubleArray(gc_ptr(String) fieldName, gc_ptr(array<double>) doubleArray) {
  AddOffset();
  m_dataOutput->WriteDoubleArray(doubleArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteStringArray(gc_ptr(String) fieldName,
                                                    gc_ptr(array<gc_ptr(String)>) stringArray) {
  AddOffset();
  m_dataOutput->WriteStringArray(stringArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteObjectArray(gc_ptr(String) fieldName,
                                                    gc_ptr(List<gc_ptr(Object)>) objectArray) {
  AddOffset();
  m_dataOutput->WriteObjectArray(objectArray);
  return this;
}

gc_ptr(IPdxWriter) PdxLocalWriter::WriteArrayOfByteArrays(gc_ptr(String) fieldName,
                                                          array<gc_ptr(array<Byte>)> ^ byteArrays) {
  AddOffset();
  m_dataOutput->WriteArrayOfByteArrays(byteArrays);
  return this;
}

// TODO:
// gc_ptr(IPdxWriter) PdxLocalWriter::WriteEnum(gc_ptr(String) fieldName, Enum e) ;
// gc_ptr(IPdxWriter) PdxLocalWriter::WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

gc_ptr(IPdxWriter) PdxLocalWriter::MarkIdentityField(gc_ptr(String) fieldName) { return this; }

gc_ptr(IPdxWriter) PdxLocalWriter::WriteField(gc_ptr(String) fieldName, gc_ptr(Object) fieldValue, gc_ptr(Type) type) {
  if (type->Equals(DotNetTypes::IntType)) {
    return this->WriteInt(fieldName, (int)fieldValue);
  } else if (type->Equals(DotNetTypes::StringType)) {
    return this->WriteString(fieldName, (gc_ptr(String))fieldValue);
  } else if (type->Equals(DotNetTypes::BooleanType)) {
    return this->WriteBoolean(fieldName, (bool)fieldValue);
  } else if (type->Equals(DotNetTypes::FloatType)) {
    return this->WriteFloat(fieldName, (float)fieldValue);
  } else if (type->Equals(DotNetTypes::DoubleType)) {
    return this->WriteDouble(fieldName, (double)fieldValue);
  } else if (type->Equals(DotNetTypes::CharType)) {
    return this->WriteChar(fieldName, (Char)fieldValue);
  } else if (type->Equals(DotNetTypes::SByteType)) {
    return this->WriteByte(fieldName, (SByte)fieldValue);
  } else if (type->Equals(DotNetTypes::ShortType)) {
    return this->WriteShort(fieldName, (short)fieldValue);
  } else if (type->Equals(DotNetTypes::LongType)) {
    return this->WriteLong(fieldName, (Int64)fieldValue);
  } else if (type->Equals(DotNetTypes::ByteArrayType)) {
    return this->WriteByteArray(fieldName, (gc_ptr(array<Byte>))fieldValue);
  } else if (type->Equals(DotNetTypes::DoubleArrayType)) {
    return this->WriteDoubleArray(fieldName, (gc_ptr(array<double>))fieldValue);
  } else if (type->Equals(DotNetTypes::FloatArrayType)) {
    return this->WriteFloatArray(fieldName, (gc_ptr(array<float>))fieldValue);
  } else if (type->Equals(DotNetTypes::ShortArrayType)) {
    return this->WriteShortArray(fieldName, (gc_ptr(array<Int16>))fieldValue);
  } else if (type->Equals(DotNetTypes::IntArrayType)) {
    return this->WriteIntArray(fieldName, (gc_ptr(array<System::Int32>))fieldValue);
  } else if (type->Equals(DotNetTypes::LongArrayType)) {
    return this->WriteLongArray(fieldName, (gc_ptr(array<Int64>))fieldValue);
  } else if (type->Equals(DotNetTypes::BoolArrayType)) {
    return this->WriteBooleanArray(fieldName, (gc_ptr(array<bool>))fieldValue);
  } else if (type->Equals(DotNetTypes::CharArrayType)) {
    return this->WriteCharArray(fieldName, (gc_ptr(array<Char>))fieldValue);
  } else if (type->Equals(DotNetTypes::StringArrayType)) {
    return this->WriteStringArray(fieldName, (gc_ptr(array<gc_ptr(String)>))fieldValue);
  } else if (type->Equals(DotNetTypes::DateType)) {
    return this->WriteDate(fieldName, (DateTime)fieldValue);
  } else if (type->Equals(DotNetTypes::ByteArrayOfArrayType)) {
    return this->WriteArrayOfByteArrays(fieldName, (array<gc_ptr(array<Byte>)> ^) fieldValue);
  } else if (type->Equals(DotNetTypes::ObjectArrayType)) {
    return this->WriteObjectArray(fieldName,
                                  safe_cast<gc_ptr(System::Collections::Generic::List<gc_ptr(Object)>)>(fieldValue));
  } else {
    return this->WriteObject(fieldName, fieldValue);
  }
}

gc_ptr(PdxType) PdxLocalWriter::GetLocalPdxType(gc_ptr(String) pdxClassName) {
  return m_dataOutput->Cache->GetPdxTypeRegistry()->GetLocalPdxType(pdxClassName);
}

gc_ptr(Apache::Geode::Client::Cache) PdxLocalWriter::Cache::get() { return m_dataOutput->Cache; }

}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
