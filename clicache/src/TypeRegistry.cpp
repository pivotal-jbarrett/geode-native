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

#include "begin_native.hpp"
#include <geode/Cache.hpp>
#include <geode/PoolManager.hpp>
#include "SerializationRegistry.hpp"
#include "CacheRegionHelper.hpp"
#include "end_native.hpp"

#include "TypeRegistry.hpp"
#include "impl/DelegateWrapper.hpp"
#include "DataOutput.hpp"
#include "DataInput.hpp"
#include "CacheableStringArray.hpp"
#include "CacheableBuiltins.hpp"
#include "impl/SafeConvert.hpp"
#include "CacheableHashTable.hpp"
#include "Struct.hpp"
#include "CacheableUndefined.hpp"
#include "CacheableObject.hpp"
#include "CacheableStack.hpp"
#include "CacheableObjectXml.hpp"
#include "CacheableHashSet.hpp"
#include "CacheableObjectArray.hpp"
#include "CacheableLinkedList.hpp"
#include "CacheableFileName.hpp"
#include "CacheableIdentityHashMap.hpp"
#include "IPdxSerializer.hpp"
#include "impl/DotNetTypes.hpp"
#include "CacheRegionHelper.hpp"
#include "Cache.hpp"
#include "Properties.hpp"
#include "cli.hpp"

using namespace apache::geode::client;
using namespace System::Reflection;
using namespace System::Reflection::Emit;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

gc_ptr(String) TypeRegistry::GetPdxTypeName(gc_ptr(String) localTypeName) {
  if (pdxTypeMapper == nullptr) {
    return localTypeName;
  }

  gc_ptr(String) pdxTypeName;
  if (localTypeNameToPdx->TryGetValue(localTypeName, pdxTypeName)) {
    return pdxTypeName;
  }

  pdxTypeName = pdxTypeMapper->ToPdxTypeName(localTypeName);
  if (pdxTypeName == nullptr) {
    throw gcnew IllegalStateException("PdxTypeName should not be null for local type " + localTypeName);
  }

  localTypeNameToPdx[localTypeName] = pdxTypeName;
  pdxTypeNameToLocal[pdxTypeName] = localTypeName;

  return pdxTypeName;
}

gc_ptr(String) TypeRegistry::GetLocalTypeName(gc_ptr(String) pdxTypeName) {
  if (pdxTypeMapper == nullptr) {
    return pdxTypeName;
  }

  gc_ptr(String) localTypeName;
  if (pdxTypeNameToLocal->TryGetValue(pdxTypeName, localTypeName)) {
    return localTypeName;
  }

  localTypeName = pdxTypeMapper->FromPdxTypeName(pdxTypeName);
  if (localTypeName == nullptr) {
    throw gcnew IllegalStateException("LocalTypeName should not be null for pdx type " + pdxTypeName);
  }

  localTypeNameToPdx[localTypeName] = pdxTypeName;
  pdxTypeNameToLocal[pdxTypeName] = localTypeName;

  return localTypeName;
}

gc_ptr(Type) TypeRegistry::GetType(gc_ptr(String) className) {
  gc_ptr(Type) type = nullptr;

  if (classNameVsType->TryGetValue(className, type)) {
    return type;
  }

  auto referedAssembly = gcnew Dictionary<gc_ptr(Assembly), bool>();
  auto MyDomain = AppDomain::CurrentDomain;
  gc_ptr(array<gc_ptr(Assembly)>) AssembliesLoaded = MyDomain->GetAssemblies();
  FOR_EACH (gc_ptr(Assembly) assembly in AssembliesLoaded) {
    type = GetTypeFromRefrencedAssemblies(className, referedAssembly, assembly);
    if (type) {
      classNameVsType[className] = type;
      return type;
    }
  }

  return type;
}

void TypeRegistry::RegisterPdxType(gc_ptr(PdxTypeFactoryMethod) creationMethod) {
  if (creationMethod == nullptr) {
    throw gcnew IllegalArgumentException(
        "Serializable.RegisterPdxType(): "
        "null PdxTypeFactoryMethod delegate passed");
  }
  gc_ptr(IPdxSerializable) obj = creationMethod();
  PdxDelegateMap[obj->GetType()->FullName] = creationMethod;
  Log::Debug("RegisterPdxType: class registered: " + obj->GetType()->FullName);
}

gc_ptr(IPdxSerializable) TypeRegistry::GetPdxType(gc_ptr(String) className) {
  gc_ptr(PdxTypeFactoryMethod) retVal = nullptr;

  if (!PdxDelegateMap->TryGetValue(className, retVal)) {
    if (pdxSerializer != nullptr) {
      return gcnew PdxWrapper(className);
    }
    try {
      gc_ptr(Object) retObj = CreateObject(className);

      gc_ptr(IPdxSerializable) retPdx = dynamic_cast<gc_ptr(IPdxSerializable)>(retObj);
      if (retPdx != nullptr) {
        return retPdx;
      }
    } catch (gc_ptr(System::Exception) ex) {
      Log::Error("Unable to create object using reflection for class: " + className + " : " + ex->Message);
    }
    throw gcnew IllegalStateException(
        "Pdx factory method (or PdxSerializer ) not registered (or don't have zero arg constructor)"
        " to create default instance for class: " +
        className);
  }

  return retVal();
}

void TypeRegistry::RegisterType(gc_ptr(TypeFactoryMethod) creationMethod, int32_t id) {
  if (creationMethod == nullptr) {
    throw gcnew IllegalArgumentException(
        "Serializable.RegisterType(): "
        "null TypeFactoryMethod delegate passed");
  }

  // adding user type as well in global builtin hashmap
  auto obj = creationMethod();
  auto dataSerializable = dynamic_cast<gc_ptr(IDataSerializable)>(obj);
  if (nullptr == dataSerializable) {
    throw gcnew IllegalArgumentException("Unknown serialization type.");
  }

  if (!ObjectIDDelegatesMap->ContainsKey(id)) {
    ObjectIDDelegatesMap->Add(id, creationMethod);
  } else {
    throw gcnew IllegalArgumentException("A class with given ID is already registered");
  }

  if (!ObjectTypeIDMap->ContainsKey(dataSerializable->GetType())) {
    ObjectTypeIDMap->Add(dataSerializable->GetType(), id);
  }
  // register the type in the DelegateMap, this is pure c# for create domain object
  Log::Fine("Registering serializable class ID " + id);
  DelegateMapGeneric[id] = creationMethod;
}

void TypeRegistry::RegisterDataSerializablePrimitiveOverrideNativeDeserialization(int8_t dsCode,
                                                                                  gc_ptr(TypeFactoryMethod)
                                                                                      creationMethod,
                                                                                  gc_ptr(Type) managedType) {
  if (creationMethod == nullptr) {
    throw gcnew IllegalArgumentException("Serializable.RegisterType(): ");
  }
  auto delegateObj = gcnew DelegateWrapperGeneric(creationMethod);
  auto nativeDelegate =
      gcnew TypeFactoryNativeMethodGeneric(delegateObj, &DelegateWrapperGeneric::NativeDelegateGeneric);

  DsCodeToDataSerializablePrimitiveTypeFactoryMethod[dsCode] = creationMethod;
  DsCodeToDataSerializablePrimitiveNativeDelegate[dsCode] = nativeDelegate;

  if (managedType != nullptr) {
    ManagedTypeToDsCode[managedType] = dsCode;
  }

  _GF_MG_EXCEPTION_TRY2
    auto&& serializationRegistry =
        CacheRegionHelper::getCacheImpl(m_cache->GetNative().get())->getSerializationRegistry();
    auto nativeDelegateFunction = static_cast<std::shared_ptr<native::Serializable> (*)()>(
        System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(nativeDelegate).ToPointer());
    serializationRegistry->setDataSerializablePrimitiveType(nativeDelegateFunction, static_cast<DSCode>(dsCode));
  _GF_MG_EXCEPTION_CATCH_ALL2
}

void TypeRegistry::RegisterDataSerializableFixedIdTypeOverrideNativeDeserialization(Int32 fixedId,
                                                                                    gc_ptr(TypeFactoryMethod)
                                                                                        creationMethod) {
  if (creationMethod == nullptr) {
    throw gcnew IllegalArgumentException("Serializable.RegisterType(): ");
  }
  auto delegateObj = gcnew DelegateWrapperGeneric(creationMethod);
  auto nativeDelegate =
      gcnew TypeFactoryNativeMethodGeneric(delegateObj, &DelegateWrapperGeneric::NativeDelegateGeneric);

  FixedIdToDataSerializableFixedIdTypeFactoryMethod[fixedId] = creationMethod;
  FixedIdToDataSerializableFixedIdNativeDelegate[fixedId] = nativeDelegate;

  Log::Finer("Registering serializable fixed ID " + fixedId);

  _GF_MG_EXCEPTION_TRY2
    auto&& serializationRegistry =
        CacheRegionHelper::getCacheImpl(m_cache->GetNative().get())->getSerializationRegistry();
    auto nativeDelegateFunction = static_cast<std::shared_ptr<native::Serializable> (*)()>(
        System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(nativeDelegate).ToPointer());
    serializationRegistry->addDataSerializableFixedIdType(static_cast<internal::DSFid>(fixedId),
                                                          nativeDelegateFunction);
  _GF_MG_EXCEPTION_CATCH_ALL2
}

void TypeRegistry::UnregisterTypeGeneric(Byte typeId) {
  DsCodeToDataSerializablePrimitiveNativeDelegate->Remove(typeId);
  _GF_MG_EXCEPTION_TRY2

    CacheRegionHelper::getCacheImpl(m_cache->GetNative().get())
        ->getSerializationRegistry()
        ->removeDataSerializableType(typeId);

  _GF_MG_EXCEPTION_CATCH_ALL2
}

void TypeRegistry::RegisterDataSerializablePrimitiveWrapper(gc_ptr(DataSerializablePrimitiveWrapperDelegate)
                                                                wrapperMethod,
                                                            int8_t dsCode, gc_ptr(System::Type) managedType) {
  DsCodeToDataSerializablePrimitiveWrapperDelegate[dsCode] = wrapperMethod;
}

void TypeRegistry::UnregisterNativesGeneric(gc_ptr(Cache) cache) {
  cache->TypeRegistry->DsCodeToDataSerializablePrimitiveNativeDelegate->Clear();
}

gc_ptr(Type) TypeRegistry::GetTypeFromRefrencedAssemblies(gc_ptr(String) className,
                                                          gc_ptr(Dictionary<gc_ptr(Assembly), bool>) referedAssembly,
                                                          gc_ptr(Assembly) currentAssembly) {
  auto type = currentAssembly->GetType(className);
  if (type != nullptr) {
    return type;
  }

  if (referedAssembly->ContainsKey(currentAssembly)) return nullptr;
  referedAssembly[currentAssembly] = true;

  // get all refrenced assembly
  gc_ptr(array<gc_ptr(AssemblyName)>) ReferencedAssemblies = currentAssembly->GetReferencedAssemblies();
  FOR_EACH (gc_ptr(AssemblyName) assembly in ReferencedAssemblies) {
    try {
      gc_ptr(Assembly) loadedAssembly = Assembly::Load(assembly);
      if (loadedAssembly != nullptr && (!referedAssembly->ContainsKey(loadedAssembly))) {
        type = GetTypeFromRefrencedAssemblies(className, referedAssembly, loadedAssembly);
        if (!type) {
          return type;
        }
      }
    } catch (gc_ptr(System::Exception)) {  // ignore
    }
  }
  return nullptr;
}

GENERIC(class TValue)
TValue wrap(std::shared_ptr<native::DataSerializablePrimitive> dataSerializablePrimitive) {
  switch (dataSerializablePrimitive->getDsCode()) {
    case native::internal::DSCode::CacheableDate: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableDate)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableBytes: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableBytes)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableDoubleArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableDoubleArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableFloatArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableFloatArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableInt16Array: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableInt16Array)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableInt32Array: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableInt32Array)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableInt64Array: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableInt64Array)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableStringArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableStringArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->GetValues());
    }
    case native::internal::DSCode::CacheableArrayList:  // Ilist generic
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableArrayList)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableLinkedList:  // LinkedList generic
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableLinkedList)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableHashTable:  // collection::hashtable
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableHashTable)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableHashMap:  // generic dictionary
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableHashMap)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableIdentityHashMap: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableIdentityHashMap)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CacheableHashSet:  // no need of it, default case should work
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableHashSet)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::internal::DSCode::CacheableLinkedHashSet:  // no need of it, default case should work
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableLinkedHashSet)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::internal::DSCode::CacheableFileName: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableFileName)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::internal::DSCode::CacheableObjectArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableObjectArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::internal::DSCode::CacheableVector:  // collection::arraylist
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableVector)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::DSFid::CacheableUndefined: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableUndefined)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::DSFid::Struct: {
      return safe_cast<TValue>(Struct::Create(dataSerializablePrimitive));
    }
    case native::internal::DSCode::CacheableStack: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableStack)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::InternalId::CacheableManagedObject: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableObject)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
    case native::internal::InternalId::CacheableManagedObjectXml: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CacheableObjectXml)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
      /*
case native::internal::DSCode::Properties: // TODO: replace with IDictionary<K, V>
{
auto ret = SafeGenericUMSerializableConvert<gc_ptr(Properties<gc_ptr(Object),
gc_ptr(Object)>)>(dataSerializablePrimitive); return safe_cast<TValue>(ret);
}
      */
    case native::internal::DSCode::BooleanArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(BooleanArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case native::internal::DSCode::CharArray: {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(CharArray)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret->Value);
    }
    case 0:  // UserFunctionExecutionException unregistered
    {
      auto ret = SafeGenericUMSerializableConvert<gc_ptr(UserFunctionExecutionException)>(dataSerializablePrimitive);
      return safe_cast<TValue>(ret);
    }
  }

  throw gcnew IllegalArgumentException("Unknown type");
}

GENERIC(class TValue)
TValue TypeRegistry::GetManagedValueGeneric(std::shared_ptr<native::Serializable> val) {
  if (val == nullptr) {
    return TValue();
  }

  if (auto dataSerializablePrimitive = std::dynamic_pointer_cast<native::DataSerializablePrimitive>(val)) {
    switch (dataSerializablePrimitive->getDsCode()) {
      case native::internal::DSCode::CacheableByte: {
        return safe_cast<TValue>(Serializable::getByte(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableBoolean: {
        return safe_cast<TValue>(Serializable::getBoolean(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableCharacter: {
        return safe_cast<TValue>(Serializable::getChar(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableDouble: {
        return safe_cast<TValue>(Serializable::getDouble(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableASCIIString:
      case native::internal::DSCode::CacheableASCIIStringHuge:
      case native::internal::DSCode::CacheableString:
      case native::internal::DSCode::CacheableStringHuge: {
        return safe_cast<TValue>(Serializable::getString(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableFloat: {
        return safe_cast<TValue>(Serializable::getFloat(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableInt16: {
        return safe_cast<TValue>(Serializable::getInt16(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableInt32: {
        return safe_cast<TValue>(Serializable::getInt32(dataSerializablePrimitive));
      }
      case native::internal::DSCode::CacheableInt64: {
        return safe_cast<TValue>(Serializable::getInt64(dataSerializablePrimitive));
      }
      default:
        return wrap<TValue>(dataSerializablePrimitive);
    }
  } else if (auto dataSerializableFixedId = std::dynamic_pointer_cast<native::DataSerializableFixedId>(val)) {
    switch (dataSerializableFixedId->getDSFID()) {
      case native::DSFid::Struct: {
        return safe_cast<TValue>(Struct::Create(val));
      }
      case native::DSFid::CacheableUndefined: {
        return safe_cast<TValue>(CacheableUndefined::Create());
      }
      default:
        break;
    }
  } else if (auto dataSerializable = std::dynamic_pointer_cast<native::DataSerializable>(val)) {
    auto ret = SafeUMSerializableConvertGeneric(dataSerializable);
    return safe_cast<TValue>(ret);
  } else if (auto pdxSerializable = std::dynamic_pointer_cast<native::PdxSerializable>(val)) {
    auto ret = SafeUMSerializablePDXConvert(pdxSerializable);
    if (auto pdxWrapper = dynamic_cast<gc_ptr(PdxWrapper)>(ret)) {
      return safe_cast<TValue>(pdxWrapper->GetObject());
    }

    return safe_cast<TValue>(ret);
  } else if (auto userFunctionExecutionException =
                 std::dynamic_pointer_cast<native::UserFunctionExecutionException>(val)) {
    return safe_cast<TValue>(gcnew UserFunctionExecutionException(userFunctionExecutionException));
  } else if (auto pdxManagedCacheableKey = std::dynamic_pointer_cast<native::PdxManagedCacheableKey>(val)) {
    auto pdx = pdxManagedCacheableKey->ptr();
    if (auto pdxWrapper = dynamic_cast<gc_ptr(PdxWrapper)>(pdx)) {
      return safe_cast<TValue>(pdxWrapper->GetObject());
    }
    return safe_cast<TValue>(pdx);
  } else {
    throw gcnew IllegalStateException("Unknown serialization type.");
  }

  throw gcnew System::Exception("not found typeid");
}

gc_ptr(Object) TypeRegistry::CreateObject(gc_ptr(String) className) {
  gc_ptr(Object) retVal = CreateObjectEx(className);

  if (retVal == nullptr) {
    auto type = GetType(className);
    if (type) {
      retVal = type->GetConstructor(Type::EmptyTypes)->Invoke(nullptr);
      return retVal;
    }
  }
  return retVal;
}

gc_ptr(Object) TypeRegistry::CreateObjectEx(gc_ptr(String) className) {
  gc_ptr(CreateNewObjectDelegate) del = nullptr;
  gc_ptr(Dictionary<gc_ptr(String), gc_ptr(CreateNewObjectDelegate)>) tmp = ClassNameVsCreateNewObjectDelegate;

  tmp->TryGetValue(className, del);

  if (del != nullptr) {
    return del();
  }
  auto type = GetType(className);
  if (type) {
    msclr::lock lockInstance(ClassNameVsCreateNewObjectLockObj);
    {
      tmp = ClassNameVsCreateNewObjectDelegate;
      tmp->TryGetValue(className, del);
      if (del != nullptr) return del();
      del = CreateNewObjectDelegateF(type);
      ClassNameVsCreateNewObjectDelegate[className] = del;
      return del();
    }
  }
  return nullptr;
}

gc_ptr(Object) TypeRegistry::GetArrayObject(gc_ptr(String) className, int length) {
  gc_ptr(Object) retArr = GetArrayObjectEx(className, length);
  if (retArr == nullptr) {
    gc_ptr(Type) type = GetType(className);
    if (type) {
      retArr = type->MakeArrayType()->GetConstructor(singleIntType)->Invoke(gcnew array<gc_ptr(Object)>(1){length});
      return retArr;
    }
  }
  return retArr;
}

gc_ptr(Object) TypeRegistry::GetArrayObjectEx(gc_ptr(String) className, int length) {
  gc_ptr(CreateNewObjectArrayDelegate) del = nullptr;
  gc_ptr(Dictionary<gc_ptr(String), gc_ptr(CreateNewObjectArrayDelegate)>) tmp =
      ClassNameVsCreateNewObjectArrayDelegate;

  tmp->TryGetValue(className, del);

  if (del != nullptr) {
    return del(length);
  }

  gc_ptr(Type) t = GetType(className);
  if (t) {
    msclr::lock lockInstance(ClassNameVsCreateNewObjectLockObj);
    {
      tmp = ClassNameVsCreateNewObjectArrayDelegate;
      tmp->TryGetValue(className, del);
      if (del != nullptr) return del(length);
      del = CreateNewObjectArrayDelegateF(t);
      tmp = gcnew Dictionary<gc_ptr(String), gc_ptr(CreateNewObjectArrayDelegate)>(
          ClassNameVsCreateNewObjectArrayDelegate);
      tmp[className] = del;
      ClassNameVsCreateNewObjectArrayDelegate = tmp;
      return del(length);
    }
  }
  return nullptr;
}

// delegate gc_ptr(Object) CreateNewObject();
// static gc_ptr(CreateNewObjectDelegate) CreateNewObjectDelegateF(gc_ptr(Type) type);
gc_ptr(TypeRegistry::CreateNewObjectDelegate) TypeRegistry::CreateNewObjectDelegateF(gc_ptr(Type) type) {
  gc_ptr(DynamicMethod) dynam =
      gcnew DynamicMethod("", Internal::DotNetTypes::ObjectType, Type::EmptyTypes, type, true);
  gc_ptr(ILGenerator) il = dynam->GetILGenerator();

  gc_ptr(ConstructorInfo) ctorInfo = type->GetConstructor(Type::EmptyTypes);
  if (ctorInfo == nullptr) {
    Log::Error("Can't deserialize {0} because this type has no public no arg constructor", type->Name);
    throw gcnew IllegalStateException("Object missing public no arg constructor");
  }

  il->Emit(OpCodes::Newobj, ctorInfo);
  il->Emit(OpCodes::Ret);

  return (gc_ptr(TypeRegistry::CreateNewObjectDelegate))dynam->CreateDelegate(createNewObjectDelegateType);
}

// delegate gc_ptr(Object) CreateNewObjectArray(int len);
gc_ptr(TypeRegistry::CreateNewObjectArrayDelegate) TypeRegistry::CreateNewObjectArrayDelegateF(gc_ptr(Type) type) {
  gc_ptr(DynamicMethod) dynam = gcnew DynamicMethod("", Internal::DotNetTypes::ObjectType, singleIntTypeA, type, true);
  gc_ptr(ILGenerator) il = dynam->GetILGenerator();

  il->Emit(OpCodes::Ldarg_0);

  il->Emit(OpCodes::Newarr, type);
  il->Emit(OpCodes::Ret);

  return (gc_ptr(TypeRegistry::CreateNewObjectArrayDelegate))dynam->CreateDelegate(createNewObjectArrayDelegateType);
}

void TypeRegistry::RegisterDataSerializablePrimitivesWrapNativeDeserialization() {
  // Register wrappers for primitive types
  // Does not intercept deserialization

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableByte::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableByte), Byte::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableBoolean::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableBoolean),
                                           Boolean::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableCharacter::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableCharacter),
                                           Char::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableDouble::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableDouble),
                                           Double::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableString::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableASCIIString),
                                           String::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableFloat::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableFloat),
                                           float ::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableInt16::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableInt16),
                                           Int16::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableInt32::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableInt32),
                                           Int32::typeid);

  RegisterDataSerializablePrimitiveWrapper(gcnew DataSerializablePrimitiveWrapperDelegate(CacheableInt64::Create),
                                           static_cast<int8_t>(native::internal::DSCode::CacheableInt64),
                                           Int64::typeid);
}

}  // namespace Client
}  // namespace Geode
}  // namespace Apache
