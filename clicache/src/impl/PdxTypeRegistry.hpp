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

#include "../begin_native.hpp"
#include "SerializationRegistry.hpp"
#include "../end_native.hpp"

#include "PdxType.hpp"
#include "PdxRemotePreservedData.hpp"
#include "../IPdxSerializable.hpp"
#include "WeakhashMap.hpp"
#include "EnumInfo.hpp"

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;

namespace Apache {
namespace Geode {
namespace Client {

namespace Internal {
private
ref class PdxTypeRegistry {
 public:
  PdxTypeRegistry(gc_ptr(Cache) cache) : m_cache(cache) {}

  // test hook;
  int testGetNumberOfPdxIds();

  // test hook
  int testNumberOfPreservedData();

  void AddPdxType(Int32 typeId, gc_ptr(PdxType) pdxType);

  gc_ptr(PdxType) GetPdxType(Int32 typeId);

  void AddLocalPdxType(gc_ptr(String) localType, gc_ptr(PdxType) pdxType);

  gc_ptr(PdxType) GetLocalPdxType(gc_ptr(String) localType);

  void SetMergedType(Int32 remoteTypeId, gc_ptr(PdxType) mergedType);

  gc_ptr(PdxType) GetMergedType(Int32 remoteTypeId);

  void SetPreserveData(gc_ptr(IPdxSerializable) obj, gc_ptr(PdxRemotePreservedData) preserveData);

  gc_ptr(PdxRemotePreservedData) GetPreserveData(gc_ptr(IPdxSerializable) obj);

  void clear();

  Int32 GetPDXIdForType(gc_ptr(Type) type, native::Pool* pool, gc_ptr(PdxType) nType, bool checkIfThere);

  Int32 GetPDXIdForType(gc_ptr(PdxType) type, native::Pool*);

  Int32 GetEnumValue(gc_ptr(EnumInfo) ei);

  gc_ptr(EnumInfo) GetEnum(Int32 enumVal);

 private:
  gc_ptr(Cache) m_cache;

  gc_ptr(Dictionary<Int32, PdxType ^>) typeIdToPdxType = gcnew Dictionary<Int32, gc_ptr(PdxType)>();

  gc_ptr(Dictionary<PdxType ^, Int32>) pdxTypeToTypeId = gcnew Dictionary<gc_ptr(PdxType), Int32>();

  gc_ptr(Dictionary<Int32, PdxType ^>) remoteTypeIdToMergedPdxType = gcnew Dictionary<Int32, gc_ptr(PdxType)>();

  gc_ptr(Dictionary<String ^, PdxType ^>) localTypeToPdxType = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxType)>();

  gc_ptr(Dictionary<EnumInfo ^, Int32>) enumToInt = gcnew Dictionary<gc_ptr(EnumInfo), Int32>();

  gc_ptr(Dictionary<Int32, EnumInfo ^>) intToEnum = gcnew Dictionary<Int32, gc_ptr(EnumInfo)>();

  gc_ptr(WeakHashMap) preserveData = gcnew WeakHashMap();

  gc_ptr(ReaderWriterLock) g_readerWriterLock = gcnew ReaderWriterLock();

  bool pdxIgnoreUnreadFields = false;
  bool pdxReadSerialized = false;
};
}  // namespace Internal
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
