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

#include "../begin_native.hpp"
#include <geode/Cache.hpp>
#include "../end_native.hpp"

#include "PdxTypeRegistry.hpp"
#include "../Serializable.hpp"
#include "PdxWrapper.hpp"
#include "../Cache.hpp"

using namespace System;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
int PdxTypeRegistry::testGetNumberOfPdxIds() { return typeIdToPdxType->Count; }

int PdxTypeRegistry::testNumberOfPreservedData() { return preserveData->Count; }

Int32 PdxTypeRegistry::GetPDXIdForType(gc_ptr(Type) pdxType, native::Pool* pool, gc_ptr(PdxType) nType,
                                       bool checkIfThere) {
  if (checkIfThere) {
    gc_ptr(PdxType) lpdx = GetLocalPdxType(nType->PdxClassName);

    if (lpdx != nullptr) return lpdx->TypeId;
  }

  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    {
      if (checkIfThere) {
        gc_ptr(PdxType) lpdx = GetLocalPdxType(nType->PdxClassName);

        if (lpdx != nullptr) return lpdx->TypeId;
      }
    }
    return Serializable::GetPDXIdForType(pool, nType, m_cache);
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
}

Int32 PdxTypeRegistry::GetPDXIdForType(gc_ptr(PdxType) pType, native::Pool* pool) {
  gc_ptr(IDictionary<gc_ptr(PdxType), Int32>) tmp = pdxTypeToTypeId;
  Int32 typeId = 0;

  tmp->TryGetValue(pType, typeId);

  if (typeId != 0) return typeId;

  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    {
      tmp = pdxTypeToTypeId;
      tmp->TryGetValue(pType, typeId);

      if (typeId != 0) return typeId;
    }
    typeId = Serializable::GetPDXIdForType(pool, pType, m_cache);
    pType->TypeId = typeId;

    auto newDict = gcnew Dictionary<gc_ptr(PdxType), Int32>(pdxTypeToTypeId);
    newDict[pType] = typeId;
    pdxTypeToTypeId = newDict;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
  PdxTypeRegistry::AddPdxType(typeId, pType);
  return typeId;
}

void PdxTypeRegistry::clear() {
  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    typeIdToPdxType = gcnew Dictionary<Int32, gc_ptr(PdxType)>();
    remoteTypeIdToMergedPdxType = gcnew Dictionary<Int32, gc_ptr(PdxType)>();
    localTypeToPdxType = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxType)>();
    pdxTypeToTypeId = gcnew Dictionary<gc_ptr(PdxType), Int32>();
    preserveData = gcnew WeakHashMap();
    intToEnum = gcnew Dictionary<Int32, gc_ptr(EnumInfo)>();
    enumToInt = gcnew Dictionary<gc_ptr(EnumInfo), Int32>();
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
}

void PdxTypeRegistry::AddPdxType(Int32 typeId, gc_ptr(PdxType) pdxType) {
  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    auto newDict = gcnew Dictionary<Int32, gc_ptr(PdxType)>(typeIdToPdxType);
    newDict[typeId] = pdxType;
    typeIdToPdxType = newDict;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
}

gc_ptr(PdxType) PdxTypeRegistry::GetPdxType(Int32 typeId) {
  try {
    // g_readerWriterLock->AcquireReaderLock(-1);
    gc_ptr(IDictionary<Int32, gc_ptr(PdxType)>) tmpDict = typeIdToPdxType;
    gc_ptr(PdxType) retVal = nullptr;

    tmpDict->TryGetValue(typeId, retVal);

    return retVal;
  } finally {
    // g_readerWriterLock->ReleaseReaderLock();
  }
}

void PdxTypeRegistry::AddLocalPdxType(gc_ptr(String) localType, gc_ptr(PdxType) pdxType) {
  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    auto tmp = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxType)>(localTypeToPdxType);
    tmp[localType] = pdxType;
    localTypeToPdxType = tmp;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
}

gc_ptr(PdxType) PdxTypeRegistry::GetLocalPdxType(gc_ptr(String) localType) {
  try {
    // g_readerWriterLock->AcquireReaderLock(-1);
    auto tmp = localTypeToPdxType;
    gc_ptr(PdxType) retVal = nullptr;

    tmp->TryGetValue(localType, retVal);

    return retVal;
  } finally {
    // g_readerWriterLock->ReleaseReaderLock();
  }
}

void PdxTypeRegistry::SetMergedType(Int32 remoteTypeId, gc_ptr(PdxType) mergedType) {
  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    auto tmp = gcnew Dictionary<Int32, gc_ptr(PdxType)>(remoteTypeIdToMergedPdxType);
    tmp[remoteTypeId] = mergedType;
    remoteTypeIdToMergedPdxType = tmp;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
}

gc_ptr(PdxType) PdxTypeRegistry::GetMergedType(Int32 remoteTypeId) {
  try {
    // g_readerWriterLock->AcquireReaderLock(-1);
    auto tmp = remoteTypeIdToMergedPdxType;
    gc_ptr(PdxType) retVal = nullptr;

    tmp->TryGetValue(remoteTypeId, retVal);

    return retVal;
  } finally {
    // g_readerWriterLock->ReleaseReaderLock();
  }
}

void PdxTypeRegistry::SetPreserveData(gc_ptr(IPdxSerializable) obj, gc_ptr(PdxRemotePreservedData) pData) {
  // preserveData[obj] = pData;
  gc_ptr(PdxWrapper) pdxWrapper = dynamic_cast<gc_ptr(PdxWrapper)>(obj);

  if (pdxWrapper != nullptr) {
    pData->Owner = pdxWrapper->GetObject();
  } else
    pData->Owner = obj;
  preserveData->Put(pData);
}

gc_ptr(PdxRemotePreservedData) PdxTypeRegistry::GetPreserveData(gc_ptr(IPdxSerializable) pdxobj) {
  gc_ptr(Object) obj = pdxobj;
  gc_ptr(PdxWrapper) pdxWrapper = dynamic_cast<gc_ptr(PdxWrapper)>(obj);
  if (pdxWrapper != nullptr) obj = pdxWrapper->GetObject();
  // gc_ptr(PdxRemotePreservedData) ppd = nullptr;

  // preserveData->TryGetValue(obj, ppd);
  gc_ptr(Object) retVal = preserveData->Get(obj);

  if (retVal != nullptr) return (gc_ptr(PdxRemotePreservedData))retVal;

  return nullptr;
}

Int32 PdxTypeRegistry::GetEnumValue(gc_ptr(EnumInfo) ei) {
  auto tmp = enumToInt;
  if (tmp->ContainsKey(ei)) return tmp[ei];
  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    tmp = enumToInt;
    if (tmp->ContainsKey(ei)) return tmp[ei];

    int val = Serializable::GetEnumValue(ei, m_cache);
    tmp = gcnew Dictionary<gc_ptr(EnumInfo), Int32>(enumToInt);
    tmp[ei] = val;
    enumToInt = tmp;
    return val;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
  return 0;
}

gc_ptr(EnumInfo) PdxTypeRegistry::GetEnum(Int32 enumVal) {
  auto tmp = intToEnum;
  gc_ptr(EnumInfo) ret = nullptr;
  tmp->TryGetValue(enumVal, ret);

  if (ret != nullptr) return ret;

  try {
    g_readerWriterLock->AcquireWriterLock(-1);
    tmp = intToEnum;
    tmp->TryGetValue(enumVal, ret);

    if (ret != nullptr) return ret;

    ret = Serializable::GetEnum(enumVal, m_cache);
    tmp = gcnew Dictionary<Int32, gc_ptr(EnumInfo)>(intToEnum);
    tmp[enumVal] = ret;
    intToEnum = tmp;
    return ret;
  } finally {
    g_readerWriterLock->ReleaseWriterLock();
  }
  return nullptr;
}  // namespace Client
}  // namespace Internal
}  // namespace Client

}  // namespace Geode
}  // namespace Apache
