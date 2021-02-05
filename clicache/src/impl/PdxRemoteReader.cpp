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

#include "PdxRemoteReader.hpp"
#include "../Log.hpp"
using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
void PdxRemoteReader::setNextOffsetToRead() {}

SByte PdxRemoteReader::ReadByte(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadByte(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      SByte retVal = PdxLocalReader::ReadByte(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

SByte PdxRemoteReader::ReadSByte(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadSByte(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      SByte retVal = PdxLocalReader::ReadSByte(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

Boolean PdxRemoteReader::ReadBoolean(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadBoolean(fieldName);  // in same order
    case -1: {
      return false;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      bool retVal = PdxLocalReader::ReadBoolean(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

Char PdxRemoteReader::ReadChar(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadChar(fieldName);  // in same order
    case -1: {
      return '\0';  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      Char retVal = PdxLocalReader::ReadChar(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

System::UInt16 PdxRemoteReader::ReadUInt16(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUInt16(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      UInt16 retVal = PdxLocalReader::ReadUInt16(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

System::UInt32 PdxRemoteReader::ReadUInt32(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUInt32(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      UInt32 retVal = PdxLocalReader::ReadUInt32(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

System::UInt64 PdxRemoteReader::ReadUInt64(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUInt64(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      UInt64 retVal = PdxLocalReader::ReadUInt64(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

short PdxRemoteReader::ReadShort(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadShort(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      Int16 retVal = PdxLocalReader::ReadShort(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

Int32 PdxRemoteReader::ReadInt(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  // Log::Debug("found choice " + choice + " " + m_currentIndex);

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadInt(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      // Log::Debug("found choice position:" + position);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      Int32 retVal = PdxLocalReader::ReadInt(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

Int64 PdxRemoteReader::ReadLong(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadLong(fieldName);  // in same order
    case -1: {
      return 0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      Int64 retVal = PdxLocalReader::ReadLong(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

float PdxRemoteReader::ReadFloat(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadFloat(fieldName);  // in same order
    case -1: {
      return 0.0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      float retVal = PdxLocalReader::ReadFloat(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

double PdxRemoteReader::ReadDouble(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadDouble(fieldName);  // in same order
    case -1: {
      return 0.0;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      double retVal = PdxLocalReader::ReadDouble(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(String) PdxRemoteReader::ReadString(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadString(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(String) retVal = PdxLocalReader::ReadString(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(String) PdxRemoteReader::ReadUTFHuge(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUTFHuge(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(String) retVal = PdxLocalReader::ReadUTFHuge(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(String) PdxRemoteReader::ReadASCIIHuge(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadASCIIHuge(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(String) retVal = PdxLocalReader::ReadASCIIHuge(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(Object) PdxRemoteReader::ReadObject(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadObject(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(Object) retVal = PdxLocalReader::ReadObject(fieldName);
      m_dataInput->ResetPdx(m_startPosition);  // force native as well
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

// TODO:
// void WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary) map );

void PdxRemoteReader::ReadCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) collection) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      PdxLocalReader::ReadCollection(fieldName, collection);  // in same order
      break;
    case -1: {
      break;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      PdxLocalReader::ReadCollection(fieldName, collection);
      m_dataInput->RewindCursorPdx(position);
    }
  }
}

System::DateTime PdxRemoteReader::ReadDate(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadDate(fieldName);  // in same order
    case -1: {
      DateTime dt(0);
      return dt;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      System::DateTime retVal = PdxLocalReader::ReadDate(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}
// void writeFile(String fieldName, File file) ;

gc_ptr(array<Boolean>) PdxRemoteReader::ReadBooleanArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadBooleanArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<bool>) retVal = PdxLocalReader::ReadBooleanArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<Char>) PdxRemoteReader::ReadCharArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadCharArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<Char>) retVal = PdxLocalReader::ReadCharArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<Byte>) PdxRemoteReader::ReadByteArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadByteArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<Byte>) retVal = PdxLocalReader::ReadByteArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<SByte>) PdxRemoteReader::ReadSByteArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadSByteArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<SByte>) retVal = PdxLocalReader::ReadSByteArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<short>) PdxRemoteReader::ReadShortArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadShortArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<Int16>) retVal = PdxLocalReader::ReadShortArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<System::UInt16>) PdxRemoteReader::ReadUnsignedShortArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUnsignedShortArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<UInt16>) retVal = PdxLocalReader::ReadUnsignedShortArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<System::Int32>) PdxRemoteReader::ReadIntArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadIntArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<Int32>) retVal = PdxLocalReader::ReadIntArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<System::UInt32>) PdxRemoteReader::ReadUnsignedIntArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUnsignedIntArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<UInt32>) retVal = PdxLocalReader::ReadUnsignedIntArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<Int64>) PdxRemoteReader::ReadLongArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadLongArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<Int64>) retVal = PdxLocalReader::ReadLongArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<System::UInt64>) PdxRemoteReader::ReadUnsignedLongArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadUnsignedLongArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<UInt64>) retVal = PdxLocalReader::ReadUnsignedLongArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<float>) PdxRemoteReader::ReadFloatArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadFloatArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<float>) retVal = PdxLocalReader::ReadFloatArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<double>) PdxRemoteReader::ReadDoubleArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadDoubleArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<double>) retVal = PdxLocalReader::ReadDoubleArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(array<gc_ptr(String)>) PdxRemoteReader::ReadStringArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadStringArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(array<gc_ptr(String)>) retVal = PdxLocalReader::ReadStringArray(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

gc_ptr(List<gc_ptr(Object)>) PdxRemoteReader::ReadObjectArray(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadObjectArray(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      gc_ptr(List<gc_ptr(Object)>) retVal = PdxLocalReader::ReadObjectArray(fieldName);
      m_dataInput->ResetPdx(m_startPosition);  // force native as well
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

array<gc_ptr(array<Byte>)> ^ PdxRemoteReader::ReadArrayOfByteArrays(gc_ptr(String) fieldName) {
  int choice = m_localToRemoteMap[m_currentIndex++];

  switch (choice) {
    case -2:
      return PdxLocalReader::ReadArrayOfByteArrays(fieldName);  // in same order
    case -1: {
      return nullptr;  // null value
    }
    default: {
      // sequence id read field and then update
      int position = m_pdxType->GetFieldPosition(choice, m_offsetsBuffer, m_offsetSize, m_serializedLength);
      m_dataInput->ResetAndAdvanceCursorPdx(position);
      array<gc_ptr(array<Byte>)> ^ retVal = PdxLocalReader::ReadArrayOfByteArrays(fieldName);
      m_dataInput->RewindCursorPdx(position);
      return retVal;
    }
  }
}

// TODO:
// void WriteEnum(gc_ptr(String) fieldName, Enum e) ;
// void WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
