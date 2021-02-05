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

#include "IPdxSerializer.hpp"
#include "Serializable.hpp"
#include <geode/CacheableBuiltins.hpp>

#include "cli.hpp"

namespace Apache {
namespace Geode {
namespace Client {

using namespace System::Collections::Concurrent;
using namespace System::Reflection;

namespace native = apache::geode::client;

ref class Cache;

/// <summary>
/// Registry for custom serializable types, both PDXSerializable and DataSerializable.
/// </summary>
CLI(PUBLIC ref class TypeRegistry) {
 public:
  TypeRegistry(gc_ptr(Cache) cache) : m_cache(cache) {
    ClassNameVsCreateNewObjectLockObj = gcnew Object();

    singleIntTypeA = gcnew array<gc_ptr(Type)>{Int32::typeid};
    singleIntType = gcnew array<gc_ptr(Type)>(1){Int32::typeid};

    ClassNameVsCreateNewObjectDelegate = gcnew Dictionary<gc_ptr(String), gc_ptr(CreateNewObjectDelegate)>();
    ClassNameVsCreateNewObjectArrayDelegate =
        gcnew Dictionary<gc_ptr(String), gc_ptr(CreateNewObjectArrayDelegate)>();

    createNewObjectDelegateType = Type::GetType("Apache.Geode.Client.TypeRegistry+CreateNewObjectDelegate");
    createNewObjectArrayDelegateType = Type::GetType("Apache.Geode.Client.TypeRegistry+CreateNewObjectArrayDelegate");
  }

  property gc_ptr(IPdxSerializer) PdxSerializer{gc_ptr(IPdxSerializer) get(){return pdxSerializer;
}

void set(gc_ptr(IPdxSerializer) pdxSerializer) { this->pdxSerializer = pdxSerializer; }
};  // namespace Client

/// <summary>
/// Register an PdxTypeMapper to map the local types to pdx types
/// </summary>
property gc_ptr(IPdxTypeMapper) PdxTypeMapper{gc_ptr(IPdxTypeMapper) get(){return pdxTypeMapper;
}  // namespace Geode

void set(gc_ptr(IPdxTypeMapper) pdxTypeMapper) { this->pdxTypeMapper = pdxTypeMapper; }
};  // namespace Apache

gc_ptr(String) GetPdxTypeName(gc_ptr(String) localTypeName);

gc_ptr(String) GetLocalTypeName(gc_ptr(String) pdxTypeName);

gc_ptr(Type) GetType(gc_ptr(String) className);

/// <summary>
/// Register an instance factory method for a given type.
/// This should be used when registering types that implement
/// IPdxSerializable.
/// </summary>
/// <param name="creationMethod">
/// the creation function to register
/// </param>
/// <exception cref="IllegalArgumentException">
/// if the method is null
/// </exception>

void RegisterPdxType(gc_ptr(PdxTypeFactoryMethod) creationMethod);

gc_ptr(IPdxSerializable) GetPdxType(gc_ptr(String) className);

/// <summary>
/// Register an instance factory method for a given type.
/// This should be used when registering types that implement
/// ISerializable.
/// </summary>
/// <param name="creationMethod">
/// the creation function to register
/// </param>
/// <exception cref="IllegalArgumentException">
/// if the method is null
/// </exception>
/// <exception cref="IllegalStateException">
/// if the typeId has already been registered, or there is an error
/// in registering the type; check <c>Utils::LastError</c> for more
/// information in the latter case.
/// </exception>
void RegisterType(gc_ptr(TypeFactoryMethod) creationMethod, int32_t id);

internal : void RegisterDataSerializablePrimitiveOverrideNativeDeserialization(int8_t dsCode,
                                                                               gc_ptr(TypeFactoryMethod)
                                                                                   creationMethod,
                                                                               gc_ptr(Type) managedType);

void RegisterDataSerializableFixedIdTypeOverrideNativeDeserialization(Int32 fixedId,
                                                                      gc_ptr(TypeFactoryMethod) creationMethod);

/// <summary>
/// Unregister the type with the given typeId
/// </summary>
/// <param name="typeId">typeId of the type to unregister.</param>
void UnregisterTypeGeneric(Byte typeId);

/// <summary>
/// This is to get manged delegates.
/// </summary>
gc_ptr(TypeFactoryMethod) GetManagedObjectFactory(System::Int64 typeId) {
  gc_ptr(TypeFactoryMethod) ret = nullptr;
  ObjectIDDelegatesMap->TryGetValue(typeId, ret);
  return ret;
}
System::Int32 GetIdForManagedType(gc_ptr(System::Type) type) {
  System::Int32 ret;
  ObjectTypeIDMap->TryGetValue(type, ret);
  return ret;
}

GENERIC(class TValue)
static TValue GetManagedValueGeneric(std::shared_ptr<native::Serializable> val);

static void RegisterDataSerializablePrimitiveWrapper(gc_ptr(DataSerializablePrimitiveWrapperDelegate) wrapperMethod,
                                                     int8_t dsCode, gc_ptr(System::Type) type);

/// <summary>
/// Internal static method to remove managed artifacts created by
/// RegisterType and RegisterWrapper methods when
/// <see cref="DistributedSystem.Disconnect" /> is called.
/// </summary>
static void UnregisterNativesGeneric(gc_ptr(Cache) cache);

void Clear() {
  pdxTypeNameToLocal->Clear();
  localTypeNameToPdx->Clear();
  classNameVsType->Clear();
  ClassNameVsCreateNewObjectDelegate->Clear();
  ClassNameVsCreateNewObjectArrayDelegate->Clear();
}

static inline int8_t GetDsCodeForManagedType(gc_ptr(Type) managedType) {
  int8_t retVal = 0;
  if (!ManagedTypeToDsCode->TryGetValue(managedType, retVal)) {
    if (managedType->IsGenericType) {
      ManagedTypeToDsCode->TryGetValue(managedType->GetGenericTypeDefinition(), retVal);
    }
  }
  return retVal;
}

inline gc_ptr(TypeFactoryMethod) GetDataSerializableFixedTypeFactoryMethodForFixedId(Int32 fixedId) {
  return FixedIdToDataSerializableFixedIdTypeFactoryMethod[fixedId];
}

inline gc_ptr(TypeFactoryMethod) GetDataSerializablePrimitiveTypeFactoryMethodForDsCode(int8_t dsCode) {
  return DsCodeToDataSerializablePrimitiveTypeFactoryMethod[dsCode];
}

static inline gc_ptr(DataSerializablePrimitiveWrapperDelegate)
    GetDataSerializablePrimitiveWrapperDelegateForDsCode(int8_t dsCode) {
  return DsCodeToDataSerializablePrimitiveWrapperDelegate[dsCode];
}

delegate gc_ptr(Object) CreateNewObjectDelegate();
gc_ptr(CreateNewObjectDelegate) CreateNewObjectDelegateF(gc_ptr(Type) type);

delegate gc_ptr(Object) CreateNewObjectArrayDelegate(int len);
gc_ptr(CreateNewObjectArrayDelegate) CreateNewObjectArrayDelegateF(gc_ptr(Type) type);

gc_ptr(Object) CreateObject(gc_ptr(String) className);
gc_ptr(Object) GetArrayObject(gc_ptr(String) className, int length);

gc_ptr(Object) CreateObjectEx(gc_ptr(String) className);
gc_ptr(Object) GetArrayObjectEx(gc_ptr(String) className, int length);

private:
gc_ptr(Cache) m_cache;
gc_ptr(IPdxSerializer) pdxSerializer;
gc_ptr(IPdxTypeMapper) pdxTypeMapper;
gc_ptr(ConcurrentDictionary<String ^, String ^>)
    pdxTypeNameToLocal = gcnew ConcurrentDictionary<gc_ptr(String), gc_ptr(String)>();
gc_ptr(ConcurrentDictionary<String ^, String ^>)
    localTypeNameToPdx = gcnew ConcurrentDictionary<gc_ptr(String), gc_ptr(String)>();
gc_ptr(ConcurrentDictionary<String ^, Type ^>)
    classNameVsType = gcnew ConcurrentDictionary<gc_ptr(String), gc_ptr(Type)>();
gc_ptr(Dictionary<String ^, PdxTypeFactoryMethod ^>)
    PdxDelegateMap = gcnew Dictionary<gc_ptr(String), gc_ptr(PdxTypeFactoryMethod)>();

gc_ptr(Dictionary<System::Int64, TypeFactoryMethod ^>)
    ObjectIDDelegatesMap = gcnew Dictionary<System::Int64, gc_ptr(TypeFactoryMethod)>();
gc_ptr(List<TypeFactoryNativeMethodGeneric ^>)
    NativeDelegatesGeneric = gcnew List<gc_ptr(TypeFactoryNativeMethodGeneric)>();
gc_ptr(Dictionary<UInt32, TypeFactoryMethod ^>)
    DelegateMapGeneric = gcnew Dictionary<UInt32, gc_ptr(TypeFactoryMethod)>();

gc_ptr(Dictionary<System::Type ^, System::Int32>)
    ObjectTypeIDMap = gcnew Dictionary<gc_ptr(System::Type), System::Int32>();

gc_ptr(Dictionary<Byte, TypeFactoryMethod ^>)
    DsCodeToDataSerializablePrimitiveTypeFactoryMethod = gcnew Dictionary<Byte, gc_ptr(TypeFactoryMethod)>();

gc_ptr(Dictionary<Byte, TypeFactoryNativeMethodGeneric ^>) DsCodeToDataSerializablePrimitiveNativeDelegate =
    gcnew Dictionary<Byte, gc_ptr(TypeFactoryNativeMethodGeneric)>();

gc_ptr(Dictionary<Int32, TypeFactoryMethod ^>)
    FixedIdToDataSerializableFixedIdTypeFactoryMethod = gcnew Dictionary<Int32, gc_ptr(TypeFactoryMethod)>();

gc_ptr(Dictionary<Int32, TypeFactoryNativeMethodGeneric ^>) FixedIdToDataSerializableFixedIdNativeDelegate =
    gcnew Dictionary<Int32, gc_ptr(TypeFactoryNativeMethodGeneric)>();

// Fixed .NET to DSCode mapping
static gc_ptr(Dictionary<System::Type ^, int8_t>)
    ManagedTypeToDsCode = gcnew Dictionary<gc_ptr(System::Type), int8_t>();

static gc_ptr(array<DataSerializablePrimitiveWrapperDelegate ^>) DsCodeToDataSerializablePrimitiveWrapperDelegate =
    gcnew array<gc_ptr(DataSerializablePrimitiveWrapperDelegate)>(128);

gc_ptr(Type) GetTypeFromRefrencedAssemblies(gc_ptr(String) className,
                                                gc_ptr(Dictionary<Assembly ^, bool>) referedAssembly,
                                                gc_ptr(Assembly) currentAssembly);

gc_ptr(Object) ClassNameVsCreateNewObjectLockObj;

gc_ptr(array<Type ^>) singleIntTypeA;
gc_ptr(array<Type ^>) singleIntType;

gc_ptr(Dictionary<String ^, CreateNewObjectDelegate ^>) ClassNameVsCreateNewObjectDelegate;
gc_ptr(Dictionary<String ^, CreateNewObjectArrayDelegate ^>) ClassNameVsCreateNewObjectArrayDelegate;

gc_ptr(Type) createNewObjectDelegateType;
gc_ptr(Type) createNewObjectArrayDelegateType;

static TypeRegistry() {
  InitializeManagedTypeToDsCode();
  RegisterDataSerializablePrimitivesWrapNativeDeserialization();
}

/// <summary>
/// Initializes a static map of .NET types to DataSerializable codes. Internally
/// we call it TypeId.
/// </summary>
static void InitializeManagedTypeToDsCode() {
  gc_ptr(Dictionary<Object ^, Object ^>) dic = gcnew Dictionary<gc_ptr(Object), gc_ptr(Object)>();
  ManagedTypeToDsCode[dic->GetType()] = static_cast<int8_t>(native::internal::DSCode::CacheableHashMap);
  ManagedTypeToDsCode[dic->GetType()->GetGenericTypeDefinition()] =
      static_cast<int8_t>(native::internal::DSCode::CacheableHashMap);

  gc_ptr(System::Collections::ArrayList) arr = gcnew System::Collections::ArrayList();
  ManagedTypeToDsCode[arr->GetType()] = static_cast<int8_t>(native::internal::DSCode::CacheableVector);

  gc_ptr(System::Collections::Generic::LinkedList<Object ^>) linketList =
      gcnew System::Collections::Generic::LinkedList<gc_ptr(Object)>();
  ManagedTypeToDsCode[linketList->GetType()] = static_cast<int8_t>(native::internal::DSCode::CacheableLinkedList);
  ManagedTypeToDsCode[linketList->GetType()->GetGenericTypeDefinition()] =
      static_cast<int8_t>(native::internal::DSCode::CacheableLinkedList);

  gc_ptr(System::Collections::Generic::IList<Object ^>) iList =
      gcnew System::Collections::Generic::List<gc_ptr(Object)>();
  ManagedTypeToDsCode[iList->GetType()] = static_cast<int8_t>(native::internal::DSCode::CacheableArrayList);
  ManagedTypeToDsCode[iList->GetType()->GetGenericTypeDefinition()] =
      static_cast<int8_t>(native::internal::DSCode::CacheableArrayList);

  // TODO: Linked list, non generic stack, some other map types and see if more

  gc_ptr(System::Collections::Generic::Stack<Object ^>) stack =
      gcnew System::Collections::Generic::Stack<gc_ptr(Object)>();
  ManagedTypeToDsCode[stack->GetType()] = static_cast<int8_t>(native::internal::DSCode::CacheableStack);
  ManagedTypeToDsCode[stack->GetType()->GetGenericTypeDefinition()] =
      static_cast<int8_t>(native::internal::DSCode::CacheableStack);

  ManagedTypeToDsCode[Byte::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableByte);
  ManagedTypeToDsCode[Boolean::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableBoolean);
  ManagedTypeToDsCode[Char::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableCharacter);
  ManagedTypeToDsCode[Double::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableDouble);
  ManagedTypeToDsCode[String::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableASCIIString);
  ManagedTypeToDsCode[float ::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableFloat);
  ManagedTypeToDsCode[Int16::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableInt16);
  ManagedTypeToDsCode[Int32::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableInt32);
  ManagedTypeToDsCode[Int64::typeid] = static_cast<int8_t>(native::internal::DSCode::CacheableInt64);
  ManagedTypeToDsCode[Type::GetType("System.Byte[]")] = static_cast<int8_t>(native::internal::DSCode::CacheableBytes);
  ManagedTypeToDsCode[Type::GetType("System.Double[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableDoubleArray);
  ManagedTypeToDsCode[Type::GetType("System.Single[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableFloatArray);
  ManagedTypeToDsCode[Type::GetType("System.Int16[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableInt16Array);
  ManagedTypeToDsCode[Type::GetType("System.Int32[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableInt32Array);
  ManagedTypeToDsCode[Type::GetType("System.Int64[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableInt64Array);
  ManagedTypeToDsCode[Type::GetType("System.String[]")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableStringArray);
  ManagedTypeToDsCode[Type::GetType("System.DateTime")] = static_cast<int8_t>(native::internal::DSCode::CacheableDate);
  ManagedTypeToDsCode[Type::GetType("System.Collections.Hashtable")] =
      static_cast<int8_t>(native::internal::DSCode::CacheableHashTable);
}

static void RegisterDataSerializablePrimitivesWrapNativeDeserialization();
}
;
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
