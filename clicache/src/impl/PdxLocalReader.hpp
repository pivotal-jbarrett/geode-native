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

#include "../geode_defs.hpp"
#include "../IPdxReader.hpp"
#include "PdxType.hpp"
//#include "../DataInput.hpp"
#include "PdxRemotePreservedData.hpp"
#include "../IPdxSerializable.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

ref class DataInput;
namespace Internal {
ref class PdxLocalReader : public IPdxReader {
 protected:
  gc_ptr(DataInput) m_dataInput;
  gc_ptr(PdxType) m_pdxType;
  System::Byte* m_startBuffer;
  Int32 m_startPosition;
  Int32 m_serializedLength;
  Int32 m_serializedLengthWithOffsets;
  Int32 m_offsetSize;
  System::Byte* m_offsetsBuffer;
  bool m_isDataNeedToPreserve;

  gc_ptr(PdxRemotePreservedData) m_pdxRemotePreserveData;

  gc_ptr(array<Int32>) m_localToRemoteMap;
  gc_ptr(array<Int32>) m_remoteToLocalMap;

  void initialize();

 public:
  PdxLocalReader(gc_ptr(DataInput) dataInput, gc_ptr(PdxType) pdxType, int pdxLen) {
    m_dataInput = dataInput;
    m_pdxType = pdxType;
    m_serializedLengthWithOffsets = pdxLen;

    m_localToRemoteMap = pdxType->GetLocalToRemoteMap(dataInput->Cache);
    m_remoteToLocalMap = pdxType->GetRemoteToLocalMap(dataInput->Cache);
    m_pdxRemotePreserveData = gcnew PdxRemotePreservedData();
    m_isDataNeedToPreserve = true;
    initialize();
  }

  gc_ptr(PdxRemotePreservedData) GetPreservedData(gc_ptr(PdxType) mergedVersion, gc_ptr(IPdxSerializable) object);

  void MoveStream();

  /// <summary>
  /// Read a byte from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual SByte ReadByte(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a signed byte from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual SByte ReadSByte(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a boolean value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Boolean ReadBoolean(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a char value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Char ReadChar(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 16-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt16 ReadUInt16(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 32-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt32 ReadUInt32(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 64-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt64 ReadUInt64(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 16-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual short ReadShort(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 32-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Int32 ReadInt(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 64-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Int64 ReadLong(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a floating point number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual float ReadFloat(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a double precision number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual double ReadDouble(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a string after java-modified UTF-8 decoding from the stream.
  /// The maximum length supported is gc_ptr(2)16-1 beyond which the string
  /// shall be truncated.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadString(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a string after java-modified UTF-8 decoding from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadUTFHuge(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a ASCII string from the stream. Where size is more than gc_ptr(2)16-1
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadASCIIHuge(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a serializable object from the data. Null objects are handled.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(Object) ReadObject(gc_ptr(String) fieldName);

  // TODO:
  // virtual void WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary) map );

  /// <summary>
  /// Read a collection from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual void ReadCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) collection);

  /// <summary>
  /// Read a Date from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::DateTime ReadDate(gc_ptr(String) fieldName);
  // virtual void writeFile(String fieldName, File file) ;

  /// <summary>
  /// Read a boolean array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Boolean>) ReadBooleanArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a char array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Char>) ReadCharArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Byte>) ReadByteArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a sbyte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<SByte>) ReadSByteArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a short from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<short>) ReadShortArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a ushort array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt16>) ReadUnsignedShortArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a int array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::Int32>) ReadIntArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a uint from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt32>) ReadUnsignedIntArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a long array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Int64>) ReadLongArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a ulong array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt64>) ReadUnsignedLongArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a float from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<float>) ReadFloatArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a double array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<double>) ReadDoubleArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a string array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<gc_ptr(String)>) ReadStringArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a object array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(List<gc_ptr(Object)>) ReadObjectArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a two-dimenesional byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual array<gc_ptr(array<Byte>)> ^ ReadArrayOfByteArrays(gc_ptr(String) fieldName);

  // TODO:
  // virtual void WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // virtual void WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

  /// <summary>
  /// Whether field is available or not.
  /// </summary>
  /// <param name="fieldName">The name of a member field.</param>
  virtual bool HasField(gc_ptr(String) fieldName);

  /// <summary>
  /// Whether field is used as identity field or not.
  /// </summary>
  /// <param name="fieldName">The name of a member field.</param>
  virtual bool IsIdentityField(gc_ptr(String) fieldName);

  virtual gc_ptr(IPdxUnreadFields) ReadUnreadFields();

  /// <summary>
  /// Reads the named field and returns its value.
  /// </summary>
  virtual gc_ptr(Object) ReadField(gc_ptr(String) fieldName, gc_ptr(Type) type);

  property gc_ptr(Apache::Geode::Client::Cache) Cache { virtual gc_ptr(Apache::Geode::Client::Cache) get(); }
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
