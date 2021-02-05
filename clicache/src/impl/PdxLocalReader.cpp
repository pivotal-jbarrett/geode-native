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

#include "PdxLocalReader.hpp"
#include "PdxTypeRegistry.hpp"
#include "../DataInput.hpp"
#include "DotNetTypes.hpp"
#include "../Cache.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
void PdxLocalReader::initialize() {
  // pdx header already read before this
  m_startBuffer = m_dataInput->GetCursor();
  m_startPosition = static_cast<int32_t>(m_dataInput->BytesRead);  // number of bytes read in c++;

  // m_serializedLengthWithOffsets = PdxHelper::ReadInt32(m_startBuffer);

  if (m_serializedLengthWithOffsets <= 0xff)
    m_offsetSize = 1;
  else if (m_serializedLengthWithOffsets <= 0xffff)
    m_offsetSize = 2;
  else
    m_offsetSize = 4;

  if (m_pdxType->NumberOfVarLenFields > 0)
    m_serializedLength = m_serializedLengthWithOffsets - ((m_pdxType->NumberOfVarLenFields - 1) * m_offsetSize);
  else
    m_serializedLength = m_serializedLengthWithOffsets;

  m_offsetsBuffer = m_startBuffer + m_serializedLength;
}

void PdxLocalReader::MoveStream() {
  // this will reset unmaged datainput as well
  m_dataInput->ResetPdx(m_startPosition + m_serializedLengthWithOffsets);
}

gc_ptr(PdxRemotePreservedData) PdxLocalReader::GetPreservedData(gc_ptr(PdxType) mergedVersion,
                                                                gc_ptr(IPdxSerializable) pdxObject) {
  int nFieldExtra = m_pdxType->NumberOfFieldsExtra;
  if (nFieldExtra > 0 && m_dataInput->Cache->GetPdxIgnoreUnreadFields() == false) {
    // m_pdxRemotePreserveData = gcnew PdxRemotePreservedData(m_pdxType!=nullptr? m_pdxType->TypeId : 0,
    // mergedVersion->TypeId,nFieldExtra);
    m_pdxRemotePreserveData->Initialize(m_pdxType != nullptr ? m_pdxType->TypeId : 0, mergedVersion->TypeId,
                                        nFieldExtra, pdxObject);

    m_localToRemoteMap = m_pdxType->GetLocalToRemoteMap(m_dataInput->Cache);
    m_remoteToLocalMap = m_pdxType->GetRemoteToLocalMap(m_dataInput->Cache);

    int currentIdx = 0;
    for (int i = 0; i < m_remoteToLocalMap->Length; i++) {
      if (m_remoteToLocalMap[i] == -1 || m_remoteToLocalMap[i] == -2)  // this field needs to preserve
      {
        int pos = m_pdxType->GetFieldPosition(i, m_offsetsBuffer, m_offsetSize, m_serializedLength);
        int nFieldPos = 0;

        if (i == m_remoteToLocalMap->Length - 1) {
          nFieldPos = m_serializedLength;
        } else {
          nFieldPos = m_pdxType->GetFieldPosition(i + 1, m_offsetsBuffer, m_offsetSize, m_serializedLength);
        }

        m_dataInput->ResetAndAdvanceCursorPdx(pos);
        m_pdxRemotePreserveData[currentIdx++] = m_dataInput->ReadBytesOnly(nFieldPos - pos);
        m_dataInput->RewindCursorPdx(pos);
      }
    }
    // Log::Debug("PdxLocalReader::GetPreservedData:: " + m_isDataNeedToPreserve);
    if (m_isDataNeedToPreserve)
      return m_pdxRemotePreserveData;
    else {
      // sPdxTypeRegistry::SetPreserveData(pdxObject, m_pdxRemotePreserveData);
    }
  }
  return nullptr;
}

gc_ptr(IPdxUnreadFields) PdxLocalReader::ReadUnreadFields() {
  // Log::Debug("PdxLocalReader::ReadUnreadFields:: " + m_isDataNeedToPreserve + " ignore property " +
  // PdxTypeRegistry::PdxIgnoreUnreadFields);
  if (m_dataInput->Cache->GetPdxIgnoreUnreadFields() == true) return nullptr;
  m_isDataNeedToPreserve = false;
  return m_pdxRemotePreserveData;
}

SByte PdxLocalReader::ReadByte(gc_ptr(String) fieldName) { return m_dataInput->ReadSByte(); }

SByte PdxLocalReader::ReadSByte(gc_ptr(String) fieldName) { return m_dataInput->ReadSByte(); }

Boolean PdxLocalReader::ReadBoolean(gc_ptr(String) fieldName) { return m_dataInput->ReadBoolean(); }

Char PdxLocalReader::ReadChar(gc_ptr(String) fieldName) { return m_dataInput->ReadChar(); }

System::UInt16 PdxLocalReader::ReadUInt16(gc_ptr(String) fieldName) { return m_dataInput->ReadUInt16(); }

System::UInt32 PdxLocalReader::ReadUInt32(gc_ptr(String) fieldName) { return m_dataInput->ReadUInt32(); }

System::UInt64 PdxLocalReader::ReadUInt64(gc_ptr(String) fieldName) { return m_dataInput->ReadUInt64(); }

short PdxLocalReader::ReadShort(gc_ptr(String) fieldName) { return m_dataInput->ReadInt16(); }

Int32 PdxLocalReader::ReadInt(gc_ptr(String) fieldName) { return m_dataInput->ReadInt32(); }

Int64 PdxLocalReader::ReadLong(gc_ptr(String) fieldName) { return m_dataInput->ReadInt64(); }

float PdxLocalReader::ReadFloat(gc_ptr(String) fieldName) { return m_dataInput->ReadFloat(); }

double PdxLocalReader::ReadDouble(gc_ptr(String) fieldName) { return m_dataInput->ReadDouble(); }

gc_ptr(String) PdxLocalReader::ReadString(gc_ptr(String) fieldName) { return m_dataInput->ReadString(); }

gc_ptr(String) PdxLocalReader::ReadUTFHuge(gc_ptr(String) fieldName) { return m_dataInput->ReadUTFHuge(); }

gc_ptr(String) PdxLocalReader::ReadASCIIHuge(gc_ptr(String) fieldName) { return m_dataInput->ReadASCIIHuge(); }

gc_ptr(Object) PdxLocalReader::ReadObject(gc_ptr(String) fieldName) { return m_dataInput->ReadObject(); }

// TODO:
// void WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary) map );

void PdxLocalReader::ReadCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) collection) {
  return m_dataInput->ReadCollection(collection);
}

System::DateTime PdxLocalReader::ReadDate(gc_ptr(String) fieldName) { return m_dataInput->ReadDate(); }
// void writeFile(String fieldName, File file) ;

gc_ptr(array<bool>) PdxLocalReader::ReadBooleanArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<bool>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadBooleanArray();
}

gc_ptr(array<Char>) PdxLocalReader::ReadCharArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<Char>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadCharArray();
}

gc_ptr(array<Byte>) PdxLocalReader::ReadByteArray(gc_ptr(String) fieldName) {
  gc_ptr(array<Byte>) arr;
  m_dataInput->ReadObject(arr);
  return arr;
}

gc_ptr(array<SByte>) PdxLocalReader::ReadSByteArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<SByte>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadSBytes();
}

gc_ptr(array<System::Int16>) PdxLocalReader::ReadShortArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<Int16>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadShortArray();
}

gc_ptr(array<System::UInt16>) PdxLocalReader::ReadUnsignedShortArray(gc_ptr(String) fieldName) {
  gc_ptr(array<UInt16>) arr;
  m_dataInput->ReadObject(arr);
  return arr;
}

gc_ptr(array<System::Int32>) PdxLocalReader::ReadIntArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<Int32>) arr;
  /// m_dataInput->ReadObject(arr);
  return m_dataInput->ReadIntArray();
}

gc_ptr(array<System::UInt32>) PdxLocalReader::ReadUnsignedIntArray(gc_ptr(String) fieldName) {
  gc_ptr(array<UInt32>) arr;
  m_dataInput->ReadObject(arr);
  return arr;
}

gc_ptr(array<Int64>) PdxLocalReader::ReadLongArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<Int64>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadLongArray();
}

gc_ptr(array<System::UInt64>) PdxLocalReader::ReadUnsignedLongArray(gc_ptr(String) fieldName) {
  gc_ptr(array<UInt64>) arr;
  m_dataInput->ReadObject(arr);
  return arr;
}

gc_ptr(array<float>) PdxLocalReader::ReadFloatArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<float>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadFloatArray();
}

gc_ptr(array<double>) PdxLocalReader::ReadDoubleArray(gc_ptr(String) fieldName) {
  // gc_ptr(array<Double>) arr;
  // m_dataInput->ReadObject(arr);
  return m_dataInput->ReadDoubleArray();
}

gc_ptr(array<String ^>) PdxLocalReader::ReadStringArray(gc_ptr(String) fieldName) {
  return m_dataInput->ReadStringArray();
}

gc_ptr(List<Object ^>) PdxLocalReader::ReadObjectArray(gc_ptr(String) fieldName) {
  return m_dataInput->ReadObjectArray();
}

array<gc_ptr(array<Byte>)> ^ PdxLocalReader::ReadArrayOfByteArrays(gc_ptr(String) fieldName) {
  return m_dataInput->ReadArrayOfByteArrays();
}

// TODO:
// void WriteEnum(gc_ptr(String) fieldName, Enum e) ;
// void WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

bool PdxLocalReader::HasField(gc_ptr(String) fieldName) { return m_pdxType->GetPdxField(fieldName) != nullptr; }

bool PdxLocalReader::IsIdentityField(gc_ptr(String) fieldName) {
  gc_ptr(PdxFieldType) pft = m_pdxType->GetPdxField(fieldName);
  return (pft != nullptr) && (pft->IdentityField);
}

gc_ptr(Object) PdxLocalReader::ReadField(gc_ptr(String) fieldName, gc_ptr(Type) type) {
  if (type->Equals(DotNetTypes::IntType)) {
    return this->ReadInt(fieldName);
  } else if (type->Equals(DotNetTypes::StringType)) {
    return this->ReadString(fieldName);
  } else if (type->Equals(DotNetTypes::BooleanType)) {
    return this->ReadBoolean(fieldName);
  } else if (type->Equals(DotNetTypes::FloatType)) {
    return this->ReadFloat(fieldName);
  } else if (type->Equals(DotNetTypes::DoubleType)) {
    return this->ReadDouble(fieldName);
  } else if (type->Equals(DotNetTypes::CharType)) {
    return this->ReadChar(fieldName);
  } else if (type->Equals(DotNetTypes::SByteType)) {
    return this->ReadByte(fieldName);
  } else if (type->Equals(DotNetTypes::ShortType)) {
    return this->ReadShort(fieldName);
  } else if (type->Equals(DotNetTypes::LongType)) {
    return this->ReadLong(fieldName);
  } else if (type->Equals(DotNetTypes::ByteArrayType)) {
    return this->ReadByteArray(fieldName);
  } else if (type->Equals(DotNetTypes::DoubleArrayType)) {
    return this->ReadDoubleArray(fieldName);
  } else if (type->Equals(DotNetTypes::FloatArrayType)) {
    return this->ReadFloatArray(fieldName);
  } else if (type->Equals(DotNetTypes::ShortArrayType)) {
    return this->ReadShortArray(fieldName);
  } else if (type->Equals(DotNetTypes::IntArrayType)) {
    return this->ReadIntArray(fieldName);
  } else if (type->Equals(DotNetTypes::LongArrayType)) {
    return this->ReadLongArray(fieldName);
  } else if (type->Equals(DotNetTypes::BoolArrayType)) {
    return this->ReadBooleanArray(fieldName);
  } else if (type->Equals(DotNetTypes::CharArrayType)) {
    return this->ReadCharArray(fieldName);
  } else if (type->Equals(DotNetTypes::StringArrayType)) {
    return this->ReadStringArray(fieldName);
  } else if (type->Equals(DotNetTypes::DateType)) {
    return this->ReadDate(fieldName);
  } else if (type->Equals(DotNetTypes::ByteArrayOfArrayType)) {
    return this->ReadArrayOfByteArrays(fieldName);
  } else if (type->Equals(DotNetTypes::ObjectArrayType)) {
    return this->ReadObjectArray(fieldName);
  } else {
    return this->ReadObject(fieldName);
    // throw gcnew IllegalStateException("ReadField unable to de-serialize  "
    //																	+ fieldName + " of "
    //+ type);
  }
}

gc_ptr(Apache::Geode::Client::Cache) PdxLocalReader::Cache::get() { return m_dataInput->Cache; }

}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
