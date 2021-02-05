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
#include "PdxLocalWriter.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

ref class DataOutput;
namespace Internal {
ref class PdxRemoteWriter : public PdxLocalWriter {
 private:
  gc_ptr(array<int>) m_remoteTolocalMap;
  Int32 m_preserveDataIdx;
  Int32 m_currentDataIdx;

  void initialize();
  void writePreserveData();

 public:
  PdxRemoteWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(PdxType) pdxType, gc_ptr(PdxRemotePreservedData) preservedData)
      : PdxLocalWriter(dataOutput, pdxType) {
    m_pdxType = pdxType;
    m_preserveData = preservedData;
    m_preserveDataIdx = 0;
    m_remoteTolocalMap = m_pdxType->GetRemoteToLocalMap(dataOutput->Cache);
    m_currentDataIdx = -1;
    m_pdxClassName = pdxType->PdxClassName;
    initialize();
  }

  PdxRemoteWriter(gc_ptr(DataOutput) dataOutput, gc_ptr(String) pdxClassName)
      : PdxLocalWriter(dataOutput, nullptr, pdxClassName) {
    m_pdxType = nullptr;
    m_preserveData = nullptr;
    m_preserveDataIdx = 0;
    // m_remoteTolocalMap = m_pdxType->GetRemoteToLocalMap();
    m_currentDataIdx = -1;
    m_pdxClassName = pdxClassName;
    initialize();
  }

  // i think on need to ovverride this local should work
  virtual void EndObjectWriting() override {
    writePreserveData();
    // write header
    PdxLocalWriter::WritePdxHeader();
  }

  virtual bool isFieldWritingStarted() override;

  /// <summary>
  /// Write a byte to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The byte to write.</param>
  virtual gc_ptr(IPdxWriter) WriteByte(gc_ptr(String) fieldName, SByte value) override;

  /// <summary>
  /// Write a signed byte to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The signed byte to write.</param>
  virtual gc_ptr(IPdxWriter) WriteSByte(gc_ptr(String) fieldName, SByte value) override;

  /// <summary>
  /// Write a boolean value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The boolean value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteBoolean(gc_ptr(String) fieldName, bool value) override;

  /// <summary>
  /// Write a char value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The char value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteChar(gc_ptr(String) fieldName, Char value) override;

  /// <summary>
  /// Write an unsigned short integer (System::Int16) to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 16-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt16(gc_ptr(String) fieldName, System::UInt16 value) override;

  /// <summary>
  /// Write an unsigned 32-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 32-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt32(gc_ptr(String) fieldName, System::UInt32 value) override;

  /// <summary>
  /// Write an unsigned 64-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The unsigned 64-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUInt64(gc_ptr(String) fieldName, System::UInt64 value) override;

  /// <summary>
  /// Write a 16-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 16-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteShort(gc_ptr(String) fieldName, short value) override;

  /// <summary>
  /// Write a 32-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 32-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteInt(gc_ptr(String) fieldName, System::Int32 value) override;

  /// <summary>
  /// Write a 64-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 64-bit integer to write.</param>
  virtual gc_ptr(IPdxWriter) WriteLong(gc_ptr(String) fieldName, Int64 value) override;

  /// <summary>
  /// Write a float to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The float value to write.</param>
  virtual gc_ptr(IPdxWriter) WriteFloat(gc_ptr(String) fieldName, float value) override;

  /// <summary>
  /// Write a double precision real number to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">
  /// The double precision real number to write.
  /// </param>
  virtual gc_ptr(IPdxWriter) WriteDouble(gc_ptr(String) fieldName, double value) override;

  /// <summary>
  /// Write a string using java-modified UTF-8 encoding to
  /// <c>IPdxWriter</c>.
  /// The maximum length supported is gc_ptr(2)16-1 beyond which the string
  /// shall be truncated.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteString(gc_ptr(String) fieldName, gc_ptr(String) value) override;

  /// <summary>
  /// Write a string using java-modified UTF-8 encoding to
  /// <c>IPdxWriter</c>.
  /// Length should be more than gc_ptr(2)16 -1.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteUTFHuge(gc_ptr(String) fieldName, gc_ptr(String) value) override;

  /// <summary>
  /// Write a string(only ASCII char) to
  /// <c>IPdxWriter</c>.
  /// Length should be more than gc_ptr(2)16 -1.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  virtual gc_ptr(IPdxWriter) WriteASCIIHuge(gc_ptr(String) fieldName, gc_ptr(String) value) override;

  /// <summary>
  /// Write an <c>Object</c> object to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="obj">The object to write.</param>
  virtual gc_ptr(IPdxWriter) WriteObject(gc_ptr(String) fieldName, gc_ptr(Object) obj) override;

  // TODO:
  // virtual gc_ptr(IPdxWriter) WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary)
  // map );

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="collection">The collection to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteCollection(gc_ptr(String) fieldName, gc_ptr(System::Collections::IList) collection) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="date">The date to write.</param>
  virtual gc_ptr(IPdxWriter) WriteDate(gc_ptr(String) fieldName, System::DateTime date) override;

  // TODO:
  // virtual gc_ptr(IPdxWriter) writeFile(String fieldName, File file) ;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="boolArray">The boolArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteBooleanArray(gc_ptr(String) fieldName, gc_ptr(array<Boolean>) boolArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="charArray">The charArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteCharArray(gc_ptr(String) fieldName, gc_ptr(array<Char>) charArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArray">The byteArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteByteArray(gc_ptr(String) fieldName, gc_ptr(array<Byte>) byteArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="sbyteArray">The sbyteArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteSByteArray(gc_ptr(String) fieldName, gc_ptr(array<SByte>) sbyteArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="shortArray">The shortArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteShortArray(gc_ptr(String) fieldName, gc_ptr(array<short>) shortArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="ushortArray">The ushortArray to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteUnsignedShortArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt16>) ushortArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="intArray">The intArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int32>) intArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="uintArray">The uintArray to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteUnsignedIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt32>) uintArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="longArray">The longArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteLongArray(gc_ptr(String) fieldName, gc_ptr(array<Int64>) longArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="ulongArray">The ulongArray to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteUnsignedLongArray(gc_ptr(String) fieldName, gc_ptr(array<System::UInt64>) ulongArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="floatArray">The floatArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteFloatArray(gc_ptr(String) fieldName, gc_ptr(array<float>) floatArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="doubleArray">The doubleArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteDoubleArray(gc_ptr(String) fieldName, gc_ptr(array<double>) doubleArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="stringArray">The stringArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteStringArray(gc_ptr(String) fieldName, gc_ptr(array<String ^>) stringArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="objectArray">The objectArray to write.</param>
  virtual gc_ptr(IPdxWriter) WriteObjectArray(gc_ptr(String) fieldName, gc_ptr(List<Object ^>) objectArray) override;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArrays">The byteArrays to write.</param>
  virtual gc_ptr(IPdxWriter)
      WriteArrayOfByteArrays(gc_ptr(String) fieldName, array<gc_ptr(array<Byte>)> ^ byteArrays) override;

  // TODO:
  // virtual gc_ptr(IPdxWriter) WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // virtual gc_ptr(IPdxWriter) WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

  virtual gc_ptr(IPdxWriter) WriteUnreadFields(gc_ptr(IPdxUnreadFields) unread) override;
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
