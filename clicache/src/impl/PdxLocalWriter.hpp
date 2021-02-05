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
#include "../IPdxWriter.hpp"
#include "PdxType.hpp"
#include "PdxRemotePreservedData.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

ref class Cache;
ref class DataOutput;

namespace Internal {
ref class PdxLocalWriter : public IPdxWriter {
 protected:
  gc_ptr(PdxType) m_pdxType;
  gc_ptr(DataOutput) m_dataOutput;
  System::Byte* m_startPosition;
  Int32 m_startPositionOffset;
  gc_ptr(String) m_domainClassName;
  gc_ptr(array<int>) m_offsets;
  Int32 m_currentOffsetIndex;
  gc_ptr(PdxRemotePreservedData) m_preserveData;
  // gc_ptr(Type)       m_pdxDomainType;
  gc_ptr(String) m_pdxClassName;

  gc_ptr(PdxType) GetLocalPdxType(gc_ptr(String) pdxClassName);

 public:
  PdxLocalWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(PdxType) pdxType);

  PdxLocalWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(PdxType) pdxType, gc_ptr(String) pdxClassName);

  void initialize();

  virtual void AddOffset();

  virtual void EndObjectWriting();

  // this is used to get pdx stream when WriteablePdxStream udpadates the field
  // It should be called after pdx stream has been written to output
  System::Byte* GetPdxStream(int& pdxLen);

  void WritePdxHeader();

  virtual void WriteOffsets(Int32 len);

  virtual Int32 calculateLenWithOffsets();

  virtual bool isFieldWritingStarted();

  Int32 getStartPositionOffset() { return m_startPositionOffset; }

  /// <summary>
  /// Write a byte to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The byte to write.</param>
  virtual gc_ptr(IPdxWriter) WriteByte(gc_ptr(String) fieldName, SByte value);

  /// <summary>
  /// Write a signed byte to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The signed byte to write.</param>
  virtual gc_ptr(IPdxWriter) WriteSByte(gc_ptr(String) fieldName, SByte value);

  /// <summary>
  /// Write a boolean value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The boolean value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteBoolean(gc_ptr(String) fieldName, Boolean value);

  /// <summary>
  /// Write a char value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The char value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteChar(gc_ptr(String) fieldName, Char value);

  /// <summary>
  /// Write an unsigned short integer (System::Int16) to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 16-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt16(gc_ptr(String) fieldName, System::UInt16 value);

  /// <summary>
  /// Write an unsigned 32-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 32-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt32(gc_ptr(String) fieldName, System::UInt32 value);

  /// <summary>
  /// Write an unsigned 64-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 64-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt64(gc_ptr(String) fieldName, System::UInt64 value);

  /// <summary>
  /// Write a 16-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 16-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteShort(gc_ptr(String) fieldName, short value);

  /// <summary>
  /// Write a 32-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 32-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteInt(gc_ptr(String) fieldName, Int32 value);

  /// <summary>
  /// Write a 64-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 64-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteLong(gc_ptr(String) fieldName, Int64 value);

  /// <summary>
  /// Write a float to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The float value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteFloat(gc_ptr(String) fieldName, float value);

  /// <summary>
  /// Write a double precision real number to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">
  /// The double precision real number to write.
  /// </param>
  virtual gc_ptr(IPdxWriter) WriteDouble(gc_ptr(String) fieldName, double value);

  /// <summary>
  /// Write a string using java-modified UTF-8 encoding to
  /// <c>IPdxWriter</c>.
  /// The maximum length supported is gc_ptr(2)16-1 beyond which the string
  /// shall be truncated.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteString(gc_ptr(String) fieldName, gc_ptr(String) value);

  /// <summary>
  /// Write a string using java-modified UTF-8 encoding to
  /// <c>IPdxWriter</c>.
  /// Length should be more than gc_ptr(2)16 -1.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUTFHuge(gc_ptr(String) fieldName, gc_ptr(String) value);

  /// <summary>
  /// Write a string(only ASCII char) to
  /// <c>IPdxWriter</c>.
  /// Length should be more than gc_ptr(2)16 -1.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteASCIIHuge(gc_ptr(String) fieldName, gc_ptr(String) value);

  /// <summary>
  /// Write an <c>Object</c> object to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="obj">The object to write.</param>
  virtual gc_ptr(IPdxWriter) WriteObject(gc_ptr(String) fieldName, gc_ptr(Object) obj);

  // TODO:
  // virtual gc_ptr(IPdxWriter) WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary)
  // map );

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="collection">The collection to write.</param>
  virtual gc_ptr(IPdxWriter) WriteCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) collection);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="date">The date to write.</param>
  virtual gc_ptr(IPdxWriter) WriteDate(gc_ptr(String) fieldName, System::DateTime date);

  // TODO:
  // virtual gc_ptr(IPdxWriter) writeFile(String fieldName, File file) ;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="boolArray">The boolArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteBooleanArray(gc_ptr(String) fieldName, gc_ptr(array<Boolean>) boolArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="charArray">The charArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteCharArray(gc_ptr(String) fieldName, gc_ptr(array<Char>) charArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArray">The byteArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteByteArray(gc_ptr(String) fieldName, gc_ptr(array<Byte>) byteArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="sbyteArray">The sbyteArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteSByteArray(gc_ptr(String) fieldName, gc_ptr(array<SByte>) sbyteArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="shortArray">The shortArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteShortArray(gc_ptr(String) fieldName, gc_ptr(array<short>) shortArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="ushortArray">The ushortArray to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteUnsignedShortArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt16>) ushortArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="intArray">The intArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int32>) intArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="uintArray">The uintArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUnsignedIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt32>) uintArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="longArray">The longArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteLongArray(gc_ptr(String) fieldName, gc_ptr(array<Int64>) longArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="ulongArray">The ulongArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUnsignedLongArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt64>) ulongArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="floatArray">The floatArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteFloatArray(gc_ptr(String) fieldName, gc_ptr(array<float>) floatArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="doubleArray">The doubleArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteDoubleArray(gc_ptr(String) fieldName, gc_ptr(array<double>) doubleArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="stringArray">The stringArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteStringArray(gc_ptr(String) fieldName, gc_ptr(array<gc_ptr(String)>) stringArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="objectArray">The objectArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteObjectArray(gc_ptr(String) fieldName, gc_ptr(List<gc_ptr(Object)>) objectArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArrays">The byteArrays to write.</param>
  virtual gc_ptr(IPdxWriter) WriteArrayOfByteArrays(gc_ptr(String) fieldName, array<gc_ptr(array<Byte>)> ^ byteArrays);

  // TODO:
  // virtual gc_ptr(IPdxWriter) WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // virtual gc_ptr(IPdxWriter) WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

  virtual gc_ptr(IPdxWriter) MarkIdentityField(gc_ptr(String) fieldName);

  virtual gc_ptr(IPdxWriter) WriteUnreadFields(gc_ptr(IPdxUnreadFields) unread);

  /// <summary>
  /// Writes the named field with the given value and type to the serialized form.
  /// This method uses the <code>fieldType</code> to determine which WriteXXX method it should call.
  /// If it can not find a specific match to a writeXXX method it will call <see cref="WriteObject(gc_ptr(String),
  /// gc_ptr(Object))">.
  ///
  /// </summary>
  /// <returns>this PdxWriter</returns>
  virtual gc_ptr(IPdxWriter) WriteField(gc_ptr(String) fieldName, gc_ptr(Object) fieldValue, gc_ptr(Type) type);

  virtual void WriteByte(Byte byte);  // for internal purpose

  property gc_ptr(Apache::Geode::Client::Cache) Cache { virtual gc_ptr(Apache::Geode::Client::Cache) get(); }
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
