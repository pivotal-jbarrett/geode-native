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
#include "PdxLocalReader.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

ref class DataInput;
namespace Internal {
ref class PdxReaderWithTypeCollector : public PdxLocalReader {
 private:
  gc_ptr(PdxType) m_newPdxType;

  void checkType(gc_ptr(String) fieldName, PdxFieldTypes typeId, gc_ptr(String) fieldType);

 public:
  PdxReaderWithTypeCollector(gc_ptr(DataInput) dataInput, gc_ptr(PdxType) pdxType, int pdxlen)
      : PdxLocalReader(dataInput, pdxType, pdxlen) {
    m_newPdxType = gcnew PdxType(pdxType->PdxClassName, true);
  }

  property gc_ptr(PdxType) LocalType {
    gc_ptr(PdxType) get() { return m_newPdxType; }
  }

  /// <summary>
  /// Read a signed byte from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual SByte ReadByte(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a signed byte from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual SByte ReadSByte(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a boolean value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual bool ReadBoolean(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a char value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Char ReadChar(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 16-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt16 ReadUInt16(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 32-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt32 ReadUInt32(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 64-bit unsigned integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::UInt64 ReadUInt64(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 16-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual short ReadShort(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 32-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::Int32 ReadInt(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a 64-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual Int64 ReadLong(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a floating point number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual float ReadFloat(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a double precision number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual double ReadDouble(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a string after java-modified UTF-8 decoding from the stream.
  /// The maximum length supported is gc_ptr(2)16-1 beyond which the string
  /// shall be truncated.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadString(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a string after java-modified UTF-8 decoding from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadUTFHuge(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a ASCII string from the stream. Where size is more than gc_ptr(2)16-1
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(String) ReadASCIIHuge(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a serializable object from the data. Null objects are handled.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(Object) ReadObject(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a Date from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual System::DateTime ReadDate(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a boolean array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Boolean>) ReadBooleanArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a char array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Char>) ReadCharArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Byte>) ReadByteArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a sbyte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<SByte>) ReadSByteArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a short from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<short>) ReadShortArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a ushort array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt16>) ReadUnsignedShortArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a int array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::Int32>) ReadIntArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a uint from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt32>) ReadUnsignedIntArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a long array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<Int64>) ReadLongArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a ulong array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<System::UInt64>) ReadUnsignedLongArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a float from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<float>) ReadFloatArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a double array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<double>) ReadDoubleArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a string array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(array<String ^>) ReadStringArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a object array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual gc_ptr(List<Object ^>) ReadObjectArray(gc_ptr(String) fieldName) override;

  /// <summary>
  /// Read a two-dimenesional byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  virtual array<gc_ptr(array<Byte>)> ^ ReadArrayOfByteArrays(gc_ptr(String) fieldName) override;

  // TODO:
  // virtual void WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // virtual void WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
