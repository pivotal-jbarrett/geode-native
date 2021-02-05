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

#include "geode_defs.hpp"
#include "IRegion.hpp"
#include "IPdxUnreadFields.hpp"
namespace Apache {
namespace Geode {
namespace Client {
ref class Cache;

/// <summary>
/// A IPdxReader will be passed to IPdxSerializable.fromData or
/// during deserialization of a PDX. The domain class needs to deserialize field members
/// using this interface. This interface is implemented by Native Client.
/// Each readXXX call will return the field's value. If the serialized
/// PDX does not contain the named field then a default value will
/// be returned. Standard Java defaults are used. For Objects this is
/// null and for primitives it is 0 or 0.0.
/// </summary>
PUBLIC interface class IPdxReader {
 public:
  /// <summary>
  /// Read a signed byte from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  SByte ReadByte(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a boolean value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  Boolean ReadBoolean(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a char value from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  Char ReadChar(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 16-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  short ReadShort(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 32-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  Int32 ReadInt(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a 64-bit integer from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  Int64 ReadLong(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a floating point number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  float ReadFloat(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a double precision number from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  double ReadDouble(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a string after java-modified UTF-8 decoding from the stream.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(String) ReadString(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a serializable object from the data. Null objects are handled.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(Object) ReadObject(gc_ptr(String) fieldName);

  // TODO:
  // void WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary) map );

  /// <summary>
  /// Read a Date from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  System::DateTime ReadDate(gc_ptr(String) fieldName);
  // void writeFile(String fieldName, File file) ;

  /// <summary>
  /// Read a boolean array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<Boolean>) ReadBooleanArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a char array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<Char>) ReadCharArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a signed byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<Byte>) ReadByteArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a short from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<short>) ReadShortArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a int array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<System::Int32>) ReadIntArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a long array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<Int64>) ReadLongArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a float from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<float>) ReadFloatArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a double array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<double>) ReadDoubleArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a string array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(array<String ^>) ReadStringArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a object array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  gc_ptr(List<Object ^>) ReadObjectArray(gc_ptr(String) fieldName);

  /// <summary>
  /// Read a two-dimenesional signed byte array from the data.
  /// </summary>
  /// <param name="fieldName">The name of a member field whose value to read.</param>
  array<gc_ptr(array<Byte>)> ^ ReadArrayOfByteArrays(gc_ptr(String) fieldName);

  // TODO:
  // void WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // void WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

  /// <summary>
  /// Whether field is available or not.
  /// </summary>
  /// <param name="fieldName">The name of a member field.</param>
  bool HasField(gc_ptr(String) fieldName);

  /// <summary>
  /// Whether field is used as identity field or not.
  /// </summary>
  /// <param name="fieldName">The name of a member field.</param>
  bool IsIdentityField(gc_ptr(String) fieldName);

  /// <summary>
  /// To preserve unread data, which get added in new version of type.
  /// </summary>
  /// <return>Unread data.</return>
  gc_ptr(IPdxUnreadFields) ReadUnreadFields();

  /// <summary>
  /// Reads the named field  of Type "type" and returns its value.
  /// </summary>
  /// <param name="fieldName">The name of a member field.</param>
  /// <param name="type">The type of a member field, which value needs to read.</param>
  gc_ptr(Object) ReadField(gc_ptr(String) fieldName, gc_ptr(Type) type);

  property gc_ptr(Apache::Geode::Client::Cache) Cache { gc_ptr(Apache::Geode::Client::Cache) get(); }
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
