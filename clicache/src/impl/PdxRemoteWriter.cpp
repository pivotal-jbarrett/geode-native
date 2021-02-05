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

#include "PdxRemoteWriter.hpp"
#include "PdxTypeRegistry.hpp"
using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
/*
 * Purpose of this class to map local type to remote type
 * If remote type will have extra field then we have to merge that field
 * if local type has field then need to put on stream
 */

void PdxRemoteWriter::writePreserveData() {
  m_currentDataIdx++;  // it starts from -1

  if (m_preserveData != nullptr) {
    while (m_currentDataIdx < m_remoteTolocalMap->Length) {
      if (m_remoteTolocalMap[m_currentDataIdx] == -1)  // need to add preserve data with offset
      {
        PdxLocalWriter::AddOffset();
        m_dataOutput->WriteBytesOnly(m_preserveData[m_preserveDataIdx++]);
        m_currentDataIdx++;
      } else if (m_remoteTolocalMap[m_currentDataIdx] == -2)  // need to add preserve data WITHOUT offset
      {
        m_dataOutput->WriteBytesOnly(m_preserveData[m_preserveDataIdx++]);
        m_currentDataIdx++;
      } else {
        break;  // continue writing local data..
      }
    }
  }
}

void PdxRemoteWriter::initialize() {
  // this is default case
  if (m_preserveData == nullptr) {
    // this needs to fix for IPdxTypeMapper
    m_pdxType = GetLocalPdxType(m_pdxClassName);
    m_offsets = gcnew array<int>(m_pdxType->NumberOfVarLenFields);
  }
}

bool PdxRemoteWriter::isFieldWritingStarted() {
  return m_currentDataIdx != -1;  // field writing NOT started. do we need this??
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUnreadFields(gc_ptr(IPdxUnreadFields) unread) {
  PdxLocalWriter::WriteUnreadFields(unread);
  m_remoteTolocalMap = m_pdxType->GetRemoteToLocalMap(m_dataOutput->Cache);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteByte(gc_ptr(String) fieldName, SByte value) {
  writePreserveData();
  PdxLocalWriter::WriteByte(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteSByte(gc_ptr(String) fieldName, SByte value) {
  writePreserveData();
  PdxLocalWriter::WriteSByte(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteBoolean(gc_ptr(String) fieldName, bool value) {
  writePreserveData();
  PdxLocalWriter::WriteBoolean(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteChar(gc_ptr(String) fieldName, Char value) {
  writePreserveData();
  PdxLocalWriter::WriteChar(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUInt16(gc_ptr(String) fieldName, System::UInt16 value) {
  writePreserveData();
  PdxLocalWriter::WriteUInt16(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUInt32(gc_ptr(String) fieldName, System::UInt32 value) {
  writePreserveData();
  PdxLocalWriter::WriteUInt32(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUInt64(gc_ptr(String) fieldName, System::UInt64 value) {
  writePreserveData();
  PdxLocalWriter::WriteUInt64(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteShort(gc_ptr(String) fieldName, System::Int16 value) {
  writePreserveData();
  PdxLocalWriter::WriteShort(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteInt(gc_ptr(String) fieldName, System::Int32 value) {
  writePreserveData();
  PdxLocalWriter::WriteInt(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteLong(gc_ptr(String) fieldName, Int64 value) {
  writePreserveData();
  PdxLocalWriter::WriteLong(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteFloat(gc_ptr(String) fieldName, float value) {
  writePreserveData();
  PdxLocalWriter::WriteFloat(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteDouble(gc_ptr(String) fieldName, double value) {
  writePreserveData();
  PdxLocalWriter::WriteDouble(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteString(gc_ptr(String) fieldName, gc_ptr(String) value) {
  writePreserveData();
  PdxLocalWriter::WriteString(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUTFHuge(gc_ptr(String) fieldName, gc_ptr(String) value) {
  writePreserveData();
  PdxLocalWriter::WriteUTFHuge(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteASCIIHuge(gc_ptr(String) fieldName, gc_ptr(String) value) {
  writePreserveData();
  PdxLocalWriter::WriteASCIIHuge(fieldName, value);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteObject(gc_ptr(String) fieldName, gc_ptr(Object) obj) {
  writePreserveData();
  PdxLocalWriter::WriteObject(fieldName, obj);
  return this;
}

// TODO:
// gc_ptr(IPdxWriter) PdxRemoteWriter::WriteMap( gc_ptr(String) fieldName,
// gc_ptr(System::Collections::IDictionary) map );

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteCollection(gc_ptr(String) fieldName,
                                                        gc_ptr(System::Collections::IList) obj) {
  writePreserveData();
  PdxLocalWriter::WriteCollection(fieldName, obj);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteDate(gc_ptr(String) fieldName, System::DateTime date) {
  writePreserveData();
  PdxLocalWriter::WriteDate(fieldName, date);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteBooleanArray(gc_ptr(String) fieldName,
                                                          gc_ptr(array<bool>) boolArray) {
  writePreserveData();
  PdxLocalWriter::WriteBooleanArray(fieldName, boolArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteCharArray(gc_ptr(String) fieldName,
                                                       gc_ptr(array<Char>) charArray) {
  writePreserveData();
  PdxLocalWriter::WriteCharArray(fieldName, charArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteByteArray(gc_ptr(String) fieldName,
                                                       gc_ptr(array<Byte>) byteArray) {
  writePreserveData();
  PdxLocalWriter::WriteByteArray(fieldName, byteArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteSByteArray(gc_ptr(String) fieldName,
                                                        gc_ptr(array<SByte>) sbyteArray) {
  writePreserveData();
  PdxLocalWriter::WriteSByteArray(fieldName, sbyteArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteShortArray(gc_ptr(String) fieldName,
                                                        gc_ptr(array<System::Int16>) shortArray) {
  writePreserveData();
  PdxLocalWriter::WriteShortArray(fieldName, shortArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUnsignedShortArray(gc_ptr(String) fieldName,
                                                                gc_ptr(array<System::UInt16>) ushortArray) {
  writePreserveData();
  PdxLocalWriter::WriteUnsignedShortArray(fieldName, ushortArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteIntArray(gc_ptr(String) fieldName,
                                                      gc_ptr(array<System::Int32>) intArray) {
  writePreserveData();
  PdxLocalWriter::WriteIntArray(fieldName, intArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUnsignedIntArray(gc_ptr(String) fieldName,
                                                              gc_ptr(array<System::UInt32>) uintArray) {
  writePreserveData();
  PdxLocalWriter::WriteUnsignedIntArray(fieldName, uintArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteLongArray(gc_ptr(String) fieldName,
                                                       gc_ptr(array<Int64>) longArray) {
  writePreserveData();
  PdxLocalWriter::WriteLongArray(fieldName, longArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteUnsignedLongArray(gc_ptr(String) fieldName,
                                                               gc_ptr(array<System::UInt64>) ulongArray) {
  writePreserveData();
  PdxLocalWriter::WriteUnsignedLongArray(fieldName, ulongArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteFloatArray(gc_ptr(String) fieldName,
                                                        gc_ptr(array<float>) floatArray) {
  writePreserveData();
  PdxLocalWriter::WriteFloatArray(fieldName, floatArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteDoubleArray(gc_ptr(String) fieldName,
                                                         gc_ptr(array<double>) doubleArray) {
  writePreserveData();
  PdxLocalWriter::WriteDoubleArray(fieldName, doubleArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteStringArray(gc_ptr(String) fieldName,
                                                         gc_ptr(array<String ^>) stringArray) {
  writePreserveData();
  PdxLocalWriter::WriteStringArray(fieldName, stringArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteObjectArray(gc_ptr(String) fieldName,
                                                         gc_ptr(List<Object ^>) objectArray) {
  writePreserveData();
  PdxLocalWriter::WriteObjectArray(fieldName, objectArray);
  return this;
}

gc_ptr(IPdxWriter) PdxRemoteWriter::WriteArrayOfByteArrays(gc_ptr(String) fieldName,
                                                               array<gc_ptr(array<Byte>)> ^ byteArrays) {
  writePreserveData();
  PdxLocalWriter::WriteArrayOfByteArrays(fieldName, byteArrays);
  return this;
}

// TODO:
// gc_ptr(IPdxWriter) PdxRemoteWriter::WriteEnum(gc_ptr(String) fieldName, Enum e) ;
// gc_ptr(IPdxWriter) PdxRemoteWriter::WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
