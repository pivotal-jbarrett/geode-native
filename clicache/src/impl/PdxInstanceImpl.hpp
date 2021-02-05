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

#include "../IPdxInstance.hpp"
#include "../IPdxSerializable.hpp"
#include "../DataInput.hpp"
#include "PdxLocalWriter.hpp"
#include "../IWritablePdxInstance.hpp"

namespace apache {
namespace geode {
namespace client {

class CachePerfStats;

}  // namespace client
}  // namespace geode
}  // namespace apache

namespace Apache {
namespace Geode {
namespace Client {
ref class Cache;

namespace Internal {
using namespace System;

namespace native = apache::geode::client;

ref class PdxType;

/// <summary>
/// Serialize the data in geode Portable Data eXchange(Pdx) Format.
/// This format provides class versioning(forward and backward compability of types) in cache.
/// This provides ability to query .NET domian objects.
/// </summary>
ref class PdxInstanceImpl : public IWritablePdxInstance, public IPdxInstance, public IPdxSerializable {
 private:
  static gc_ptr(array<SByte>) Boolean_DefaultBytes = gcnew array<SByte>{0};
  static gc_ptr(array<SByte>) Byte_DefaultBytes = gcnew array<SByte>{0};
  static gc_ptr(array<SByte>) Char_DefaultBytes = gcnew array<SByte>{0, 0};
  static gc_ptr(array<SByte>) Short_DefaultBytes = gcnew array<SByte>{0, 0};
  static gc_ptr(array<SByte>) Int_DefaultBytes = gcnew array<SByte>{0, 0, 0, 0};
  static gc_ptr(array<SByte>) Long_DefaultBytes = gcnew array<SByte>{0, 0, 0, 0, 0, 0, 0, 0};
  static gc_ptr(array<SByte>) Float_DefaultBytes = gcnew array<SByte>{0, 0, 0, 0};
  static gc_ptr(array<SByte>) Double_DefaultBytes = gcnew array<SByte>{0, 0, 0, 0, 0, 0, 0, 0};
  static gc_ptr(array<SByte>) Date_DefaultBytes = gcnew array<SByte>{-1, -1, -1, -1, -1, -1, -1, -1};
  static gc_ptr(array<SByte>) String_DefaultBytes = gcnew array<SByte>{
      static_cast<SByte>(apache::geode::client::internal::DSCode::CacheableNullString)};
  static gc_ptr(array<SByte>) Object_DefaultBytes = gcnew array<SByte>{
      static_cast<SByte>(apache::geode::client::internal::DSCode::NullObj)};
  static gc_ptr(array<SByte>) NULL_ARRAY_DefaultBytes = gcnew array<SByte>{-1};

  static gc_ptr(PdxFieldType) Default_PdxFieldType = gcnew PdxFieldType("default", "default", -1, -1 /*field index*/,
                                                                        false, 1, -1 /*var len field idx*/);

  bool hasDefaultBytes(gc_ptr(PdxFieldType) pField, gc_ptr(DataInput) dataInput, int start, int end);
  bool compareDefaulBytes(gc_ptr(DataInput) dataInput, int start, int end, gc_ptr(array<SByte>) defaultBytes);

  void cleanup();

  native::CachePerfStats* m_cachePerfStats;
  System::Byte* m_buffer;
  int m_bufferLength;
  int m_typeId;
  bool m_own;
  gc_ptr(PdxType) m_pdxType;
  gc_ptr(Apache::Geode::Client::Cache) m_cache;

  CLI(internal:)

  gc_ptr(Dictionary<gc_ptr(String), gc_ptr(Object)>) m_updatedFields;

  gc_ptr(Object) readField(gc_ptr(DataInput) dataInput, gc_ptr(String) fieldName, int typeId);

  bool checkType(gc_ptr(Type) fieldType, int typeId);

  void writeField(gc_ptr(IPdxWriter) writer, gc_ptr(String) fieldName, int typeId, gc_ptr(Object) value);

  int getOffset(gc_ptr(DataInput) dataInput, gc_ptr(PdxType) pt, int sequenceId);

  int getSerializedLength(gc_ptr(DataInput) dataInput, gc_ptr(PdxType) pt);

  void writeUnmodifieldField(gc_ptr(DataInput) dataInput, int startPos, int endPos, gc_ptr(PdxLocalWriter) localWriter);

  int getNextFieldPosition(gc_ptr(DataInput) dataInput, int fieldId, gc_ptr(PdxType) pt);

  gc_ptr(IList<gc_ptr(PdxFieldType)>) getIdentityPdxFields(gc_ptr(PdxType) pt);

  bool isPrimitiveArray(gc_ptr(Object) object);

  int getRawHashCode(gc_ptr(PdxType) pt, gc_ptr(PdxFieldType) pField, gc_ptr(DataInput) dataInput);

  static int deepArrayHashCode(gc_ptr(Object) obj);

  GENERIC(class T)
  WHERE(T, System::Collections::ICollection, System::Collections::IList, System::Collections::IEnumerable)
  static int primitiveArrayHashCode(T objArray);

  static int enumerableHashCode(gc_ptr(System::Collections::IEnumerable) enumObj);

  static int enumerateDictionary(gc_ptr(System::Collections::IDictionary) iDict);

  void setOffsetForObject(gc_ptr(DataInput) dataInput, gc_ptr(PdxType) pt, int sequenceId);

  static int comparePdxField(gc_ptr(PdxFieldType) a, gc_ptr(PdxFieldType) b);

  void equatePdxFields(gc_ptr(IList<gc_ptr(PdxFieldType)>) my, gc_ptr(IList<gc_ptr(PdxFieldType)>) other);

  // bool compareRawBytes(gc_ptr(PdxInstanceImpl) other, gc_ptr(PdxType) myPT,  gc_ptr(PdxFieldType) myF,
  // gc_ptr(PdxType) otherPT,  gc_ptr(PdxFieldType) otherF);
  bool compareRawBytes(gc_ptr(PdxInstanceImpl) other, gc_ptr(PdxType) myPT, gc_ptr(PdxFieldType) myF,
                       gc_ptr(DataInput) myDataInput, gc_ptr(PdxType) otherPT, gc_ptr(PdxFieldType) otherF,
                       gc_ptr(DataInput) otherDataInput);

  static bool enumerateDictionaryForEqual(gc_ptr(System::Collections::IDictionary) iDict,
                                          gc_ptr(System::Collections::IDictionary) otherIDict);

  static bool enumerableEquals(gc_ptr(System::Collections::IEnumerable) enumObj,
                               gc_ptr(System::Collections::IEnumerable) enumOtherObj);

  static bool deepArrayEquals(gc_ptr(Object) obj, gc_ptr(Object) otherObj);

  void updatePdxStream(System::Byte* newPdxStream, int len);

 public:
  PdxInstanceImpl(System::Byte* buffer, int length, int typeId, bool own, gc_ptr(Apache::Geode::Client::Cache) cache) {
    m_buffer = buffer;
    m_bufferLength = length;
    m_typeId = typeId;
    m_updatedFields = nullptr;
    m_own = own;
    m_pdxType = nullptr;
    m_cache = cache;
  }

  // for pdxInstance factory
  PdxInstanceImpl(gc_ptr(Dictionary<gc_ptr(String), gc_ptr(Object)>) fieldVsValue, gc_ptr(PdxType) pdxType,
                  gc_ptr(Cache) cache);

  ~PdxInstanceImpl() { cleanup(); }
  !PdxInstanceImpl() { cleanup(); }

  gc_ptr(PdxType) getPdxType();

  void setPdxId(Int32 typeId);

  virtual gc_ptr(String) GetClassName();

  virtual gc_ptr(Object) GetObject();

  virtual bool HasField(gc_ptr(String) fieldName);

  virtual gc_ptr(IList<gc_ptr(String)>) GetFieldNames();

  virtual bool IsIdentityField(gc_ptr(String) fieldName);

  virtual gc_ptr(Object) GetField(gc_ptr(String) fieldName);

  virtual bool Equals(gc_ptr(Object) other) override;

  virtual int GetHashCode() override;

  virtual gc_ptr(String) ToString() override;

  virtual gc_ptr(IWritablePdxInstance) CreateWriter();

  virtual void SetField(gc_ptr(String) fieldName, gc_ptr(Object) value);

  virtual void ToData(gc_ptr(IPdxWriter) writer);

  virtual void FromData(gc_ptr(IPdxReader) reader);
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
