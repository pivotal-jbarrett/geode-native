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

#include "PdxFieldType.hpp"

#include "../begin_native.hpp"
#include <PdxFieldType.hpp>
#include "../end_native.hpp"

using namespace System;
using namespace System::Collections::Generic;

namespace Apache {
namespace Geode {
namespace Client {

ref class DataOutput;
ref class DataInput;

namespace Internal {
private
ref class PdxType : public IDataSerializableInternal {
 private:
  gc_ptr(Object) m_lockObj;
  static const gc_ptr(String) m_javaPdxClass = "org.apache.geode.pdx.internal.PdxType";
  gc_ptr(IList<gc_ptr(PdxFieldType)>) m_pdxFieldTypes;
  gc_ptr(IList<gc_ptr(PdxType)>) m_otherVersions;
  Int32 m_cachedHashcode;

  // gc_ptr(Type)                 m_pdxDomainType;
  gc_ptr(String) m_className;
  Int32 m_geodeTypeId;
  bool m_isLocal;
  Int32 m_numberOfVarLenFields;
  Int32 m_varLenFieldIdx;

  Int32 m_numberOfFieldsExtra;

  gc_ptr(array<Int32>) m_remoteToLocalFieldMap;
  gc_ptr(array<Int32>) m_localToRemoteFieldMap;

  gc_ptr(array<Int32, 2>) m_positionMap;
  gc_ptr(IDictionary<gc_ptr(String), gc_ptr(PdxFieldType)>) m_fieldNameVsPdxType;

  bool m_isVarLenFieldAdded;
  bool m_noJavaClass;

  void initRemoteToLocal(gc_ptr(Cache) cache);
  void initLocalToRemote(gc_ptr(Cache) cache);
  // first has more fields than second
  gc_ptr(PdxType) isContains(gc_ptr(PdxType) first, gc_ptr(PdxType) second);
  gc_ptr(PdxType) clone();

  void generatePositionMap();
  Int32 variableLengthFieldPosition(gc_ptr(PdxFieldType) varLenField, System::Byte* offsetPosition, Int32 offsetSize,
                                    Int32 pdxStreamlen);
  Int32 fixedLengthFieldPosition(gc_ptr(PdxFieldType) fixLenField, System::Byte* offsetPosition, Int32 offsetSize,
                                 Int32 pdxStreamlen);

  gc_ptr(PdxType) isLocalTypeContains(gc_ptr(PdxType) otherType);
  gc_ptr(PdxType) isRemoteTypeContains(gc_ptr(PdxType) localType);

 public:
  PdxType() {
    m_cachedHashcode = 0;
    m_lockObj = gcnew Object();
    m_pdxFieldTypes = gcnew List<gc_ptr(PdxFieldType)>();
    m_otherVersions = gcnew List<gc_ptr(PdxType)>();
    m_isLocal = false;
    m_numberOfVarLenFields = 0;
    m_varLenFieldIdx = 0;  // start with 0
    m_isVarLenFieldAdded = false;
    m_fieldNameVsPdxType = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxFieldType)>();
    m_noJavaClass = false;
    m_geodeTypeId = 0;
    // m_pdxDomainType = nullptr;
  }

  PdxType(gc_ptr(String) pdxDomainClassName, bool isLocal) {
    m_cachedHashcode = 0;
    m_lockObj = gcnew Object();
    m_pdxFieldTypes = gcnew List<gc_ptr(PdxFieldType)>();
    m_otherVersions = gcnew List<gc_ptr(PdxType)>();
    //  m_className = className;
    // m_pdxDomainType = pdxDomainType;
    m_className = pdxDomainClassName;
    m_isLocal = isLocal;
    m_numberOfVarLenFields = 0;
    m_varLenFieldIdx = 0;  // start with 0
    m_isVarLenFieldAdded = false;
    m_fieldNameVsPdxType = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxFieldType)>();
    m_noJavaClass = false;
    m_geodeTypeId = 0;
  }

  static gc_ptr(ISerializable) CreateDeserializable() { return gcnew PdxType(); }
  property Int32 TotalVarLenFields {
    Int32 get() { return m_numberOfVarLenFields; };
  }
  property Int32 Totalfields {
    Int32 get() { return m_pdxFieldTypes->Count; };
  }
  property gc_ptr(IList<gc_ptr(PdxFieldType)>) PdxFieldList {
    gc_ptr(IList<gc_ptr(PdxFieldType)>) get() { return m_pdxFieldTypes; }
  }
  property Int32 TypeId {
    Int32 get() { return m_geodeTypeId; }
    void set(Int32 value) { m_geodeTypeId = value; }
  }

  property gc_ptr(String) PdxClassName {
    gc_ptr(String) get() { return m_className; }
    void set(gc_ptr(String) className) { m_className = className; }
  }
  property Int32 NumberOfFieldsExtra {
    Int32 get() { return m_numberOfFieldsExtra; }
  }

  gc_ptr(PdxFieldType) GetPdxField(gc_ptr(String) fieldName) {
    gc_ptr(PdxFieldType) retVal = nullptr;

    m_fieldNameVsPdxType->TryGetValue(fieldName, retVal);

    return retVal;
  }
  void AddOtherVersion(gc_ptr(PdxType) otherVersion) { m_otherVersions->Add(otherVersion); }

  gc_ptr(array<int>) GetLocalToRemoteMap(gc_ptr(Cache) cache);
  gc_ptr(array<int>) GetRemoteToLocalMap(gc_ptr(Cache) cache);
  property Int32 NumberOfVarLenFields {
    Int32 get() { return m_numberOfVarLenFields; }
  }
  property bool IsLocal {
    bool get() { return m_isLocal; }
    void set(bool val) { m_isLocal = val; }
  }
  virtual void ToData(gc_ptr(DataOutput) output);
  virtual void FromData(gc_ptr(DataInput) input);
  virtual property System::UInt64 ObjectSize {
    System::UInt64 get() { return 0; }
  }
  virtual gc_ptr(String) ToString() override { return "PdxType"; }
  void AddFixedLengthTypeField(gc_ptr(String) fieldName, gc_ptr(String) className, PdxFieldTypes typeId, Int32 size);
  void AddVariableLengthTypeField(gc_ptr(String) fieldName, gc_ptr(String) className, PdxFieldTypes typeId);
  void InitializeType(gc_ptr(Cache) cache);
  gc_ptr(PdxType) MergeVersion(gc_ptr(PdxType) otherVersion);
  Int32 GetFieldPosition(gc_ptr(String) fieldName, System::Byte* offsetPosition, Int32 offsetSize, Int32 pdxStreamlen);
  Int32 GetFieldPosition(Int32 fieldIdx, System::Byte* offsetPosition, Int32 offsetSize, Int32 pdxStreamlen);

  virtual bool Equals(gc_ptr(Object) otherType) override;
  virtual Int32 GetHashCode() override;
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode

}  // namespace Apache
