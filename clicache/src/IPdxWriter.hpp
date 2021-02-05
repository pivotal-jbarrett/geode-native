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
//#include "IRegion.hpp"
#include "IPdxUnreadFields.hpp"
using namespace System;
using namespace System::Collections::Generic;
namespace Apache {
namespace Geode {
namespace Client {
ref class Cache;

/// <summary>
/// A IPdxWriter will be passed to IPdxSerializable.toData
/// when it is serializing the domain class. The domain class needs to serialize member
/// fields using this interface. This interface is implemented
/// by Native Client.
/// </summary>
PUBLIC interface class IPdxWriter {
 public:
  /// <summary>
  /// Write a byte to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The byte to write.</param>
  gc_ptr(IPdxWriter) WriteByte(gc_ptr(String) fieldName, SByte value);

  /// <summary>
  /// Write a boolean value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The boolean value to write.</param>
  gc_ptr(IPdxWriter) WriteBoolean(gc_ptr(String) fieldName, Boolean value);

  /// <summary>
  /// Write a char value to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The char value to write.</param>
  gc_ptr(IPdxWriter) WriteChar(gc_ptr(String) fieldName, Char value);

  /// <summary>
  /// Write a 16-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 16-bit integer to write.</param>
  gc_ptr(IPdxWriter) WriteShort(gc_ptr(String) fieldName, Int16 value);

  /// <summary>
  /// Write a 32-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 32-bit integer to write.</param>
  gc_ptr(IPdxWriter) WriteInt(gc_ptr(String) fieldName, Int32 value);

  /// <summary>
  /// Write a 64-bit integer to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The 64-bit integer to write.</param>
  gc_ptr(IPdxWriter) WriteLong(gc_ptr(String) fieldName, Int64 value);

  /// <summary>
  /// Write a float to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The float value to write.</param>
  gc_ptr(IPdxWriter) WriteFloat(gc_ptr(String) fieldName, float value);

  /// <summary>
  /// Write a double precision real number to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">
  /// The double precision real number to write.
  /// </param>
  gc_ptr(IPdxWriter) WriteDouble(gc_ptr(String) fieldName, double value);

  /// <summary>
  /// Write a string using java-modified UTF-8 encoding to
  /// <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="value">The UTF encoded string to write.</param>
  gc_ptr(IPdxWriter) WriteString(gc_ptr(String) fieldName, gc_ptr(String) value);

  /// <summary>
  /// Write an <c>Object</c> object to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="obj">The object to write.</param>
  gc_ptr(IPdxWriter) WriteObject(gc_ptr(String) fieldName, gc_ptr(Object) obj);

  // TODO:
  // gc_ptr(IPdxWriter) WriteMap( gc_ptr(String) fieldName, gc_ptr(System::Collections::IDictionary) map );

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="date">The date to write.</param>
  gc_ptr(IPdxWriter) WriteDate(gc_ptr(String) fieldName, System::DateTime date);

  // TODO:
  // gc_ptr(IPdxWriter) writeFile(String fieldName, File file) ;

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="boolArray">The boolArray to write.</param>
  gc_ptr(IPdxWriter) WriteBooleanArray(gc_ptr(String) fieldName, gc_ptr(array<bool>) boolArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="charArray">The charArray to write.</param>
  gc_ptr(IPdxWriter) WriteCharArray(gc_ptr(String) fieldName, gc_ptr(array<Char>) charArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArray">The byteArray to write.</param>
  gc_ptr(IPdxWriter) WriteByteArray(gc_ptr(String) fieldName, gc_ptr(array<Byte>) byteArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="shortArray">The shortArray to write.</param>
  gc_ptr(IPdxWriter) WriteShortArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int16>) shortArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="intArray">The intArray to write.</param>
  gc_ptr(IPdxWriter) WriteIntArray(gc_ptr(String) fieldName, gc_ptr(array<System::Int32>) intArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="longArray">The longArray to write.</param>
  gc_ptr(IPdxWriter) WriteLongArray(gc_ptr(String) fieldName, gc_ptr(array<Int64>) longArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="floatArray">The floatArray to write.</param>
  gc_ptr(IPdxWriter) WriteFloatArray(gc_ptr(String) fieldName, gc_ptr(array<float>) floatArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="doubleArray">The doubleArray to write.</param>
  gc_ptr(IPdxWriter) WriteDoubleArray(gc_ptr(String) fieldName, gc_ptr(array<double>) doubleArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="stringArray">The stringArray to write.</param>
  gc_ptr(IPdxWriter) WriteStringArray(gc_ptr(String) fieldName, gc_ptr(array<String ^>) stringArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="objectArray">The objectArray to write.</param>
  gc_ptr(IPdxWriter) WriteObjectArray(gc_ptr(String) fieldName, gc_ptr(List<Object ^>) objectArray);

  /// <summary>
  /// Write an collection to the <c>IPdxWriter</c>.
  /// </summary>
  /// <param name="fieldName">The name of the field associated with the value.</param>
  /// <param name="byteArrays">The byteArrays to write.</param>
  gc_ptr(IPdxWriter)
      WriteArrayOfByteArrays(gc_ptr(String) fieldName, array<gc_ptr(array<Byte>)> ^ byteArrays);

  // TODO:
  // gc_ptr(IPdxWriter) WriteEnum(gc_ptr(String) fieldName, Enum e) ;
  // gc_ptr(IPdxWriter) WriteInetAddress(gc_ptr(String) fieldName, InetAddress address);

  /// <summary>
  /// Indicate that the given field name should be included in hashCode and equals checks
  /// of this object on a server that is using {@link CacheFactory#setPdxReadSerialized(boolean)}
  /// or when a client executes a query on a server.
  ///
  /// The fields that are marked as identity fields are used to generate the hashCode and
  /// equals methods of {@link PdxInstance}. Because of this, the identity fields should themselves
  /// either be primatives, or implement hashCode and equals.
  ///
  /// If no fields are set as identity fields, then all fields will be used in hashCode and equals
  /// checks.
  ///
  /// The identity fields should make marked after they are written using a write* method.
  /// </summary>
  /// <param name="fieldName"> the name of the field that should be used in the as part of the identity.</param>
  /// <returns>this PdxWriter</returns>

  gc_ptr(IPdxWriter) MarkIdentityField(gc_ptr(String) fieldName);

  /// <summary>
  /// To append unread data with updated data.
  ///
  /// </summary>
  /// <returns>this PdxWriter</returns>
  gc_ptr(IPdxWriter) WriteUnreadFields(gc_ptr(IPdxUnreadFields) unread);

  /// <summary>
  /// Writes the named field with the given value and type to the serialized form.
  /// This method uses the <code>fieldType</code> to determine which WriteXXX method it should call.
  /// If it can not find a specific match to a writeXXX method it will call <see cref="WriteObject(gc_ptr(String),
  /// gc_ptr(Object))">.
  ///
  /// </summary>
  /// <returns>this PdxWriter</returns>
  gc_ptr(IPdxWriter) WriteField(gc_ptr(String) fieldName, gc_ptr(Object) fieldValue, gc_ptr(Type) type);

  property gc_ptr(Apache::Geode::Client::Cache) Cache { gc_ptr(Apache::Geode::Client::Cache) get(); }
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
