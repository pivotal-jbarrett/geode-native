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

#include "Cache.hpp"
#include "ReflectionBasedAutoSerializer.hpp"
#include "PdxIdentityFieldAttribute.hpp"
#include "Serializable.hpp"
#pragma warning(disable : 4091)
#include <msclr/lock.h>
#include "ExceptionTypes.hpp"
#include "impl/DotNetTypes.hpp"
#include "TypeRegistry.hpp"

namespace Apache {
namespace Geode {
namespace Client {
ref class FieldWrapper {
 private:
  // static readonly Module Module = typeof(Program).Module;
  static gc_ptr(array<gc_ptr(Type)>) oneObj = gcnew array<gc_ptr(Type)>(1){Type::GetType("System.Object")};
  static gc_ptr(array<gc_ptr(Type)>) twoObj = gcnew array<gc_ptr(Type)>(2){Type::GetType("System.Object"),
                                                                           Type::GetType("System.Object")};
  delegate void MySetter(gc_ptr(Object) t1, gc_ptr(Object) t2);
  delegate gc_ptr(Object) MyGetter(gc_ptr(Object) t1);

  static gc_ptr(Type) setterDelegateType = Type::GetType("Apache.Geode.Client.FieldWrapper+MySetter");
  static gc_ptr(Type) getterDelegateType = Type::GetType("Apache.Geode.Client.FieldWrapper+MyGetter");

  gc_ptr(FieldInfo) m_fi;
  gc_ptr(String) m_fieldName;
  bool m_isIdentityField;
  FieldType m_fieldType;
  int m_pdxType;

  gc_ptr(MyGetter) m_getter;
  gc_ptr(MySetter) m_setter;

  static gc_ptr(MySetter) createFieldSetter(gc_ptr(FieldInfo) fieldInfo) {
    gc_ptr(DynamicMethod) dynam =
        gcnew DynamicMethod("", Internal::DotNetTypes::VoidType, twoObj, fieldInfo->DeclaringType, true);
    gc_ptr(ILGenerator) il = dynam->GetILGenerator();

    if (!fieldInfo->IsStatic) pushInstance(il, fieldInfo->DeclaringType);

    il->Emit(OpCodes::Ldarg_1);
    unboxIfNeeded(il, fieldInfo->FieldType);
    il->Emit(OpCodes::Stfld, fieldInfo);
    il->Emit(OpCodes::Ret);

    return (gc_ptr(MySetter))dynam->CreateDelegate(setterDelegateType);
  }

  static gc_ptr(MyGetter) createFieldGetter(gc_ptr(FieldInfo) fieldInfo) {
    gc_ptr(DynamicMethod) dynam =
        gcnew DynamicMethod("", Internal::DotNetTypes::ObjectType, oneObj, fieldInfo->DeclaringType, true);
    gc_ptr(ILGenerator) il = dynam->GetILGenerator();

    if (!fieldInfo->IsStatic) pushInstance(il, fieldInfo->DeclaringType);

    il->Emit(OpCodes::Ldfld, fieldInfo);
    boxIfNeeded(il, fieldInfo->FieldType);
    il->Emit(OpCodes::Ret);

    return (gc_ptr(MyGetter))dynam->CreateDelegate(getterDelegateType);
  }

  static void boxIfNeeded(gc_ptr(ILGenerator) il, gc_ptr(Type) type) {
    if (type->IsValueType) il->Emit(OpCodes::Box, type);
  }

  static void pushInstance(gc_ptr(ILGenerator) il, gc_ptr(Type) type) {
    il->Emit(OpCodes::Ldarg_0);
    if (type->IsValueType) il->Emit(OpCodes::Unbox, type);
  }

  static void unboxIfNeeded(gc_ptr(ILGenerator) il, gc_ptr(Type) type) {
    if (type->IsValueType) il->Emit(OpCodes::Unbox_Any, type);
  }

 public:
  FieldWrapper(gc_ptr(FieldInfo) fi, gc_ptr(String) fieldName, bool isIdentityField, FieldType fieldtype) {
    m_fi = fi;
    m_fieldName = fieldName;
    m_isIdentityField = isIdentityField;
    m_fieldType = fieldtype;

    m_setter = createFieldSetter(fi);
    m_getter = createFieldGetter(fi);
  }

  property bool isIdentityField {
    bool get() { return m_isIdentityField; }
  }

  property gc_ptr(Type) FType {
    gc_ptr(Type) get() { return m_fi->FieldType; }
  }

  property gc_ptr(String) FieldName {
    gc_ptr(String) get() { return m_fieldName; }
  }

  property gc_ptr(FieldInfo) FI {
    gc_ptr(FieldInfo) get() { return m_fi; }
  }

  void SetFieldValue(gc_ptr(Object) parent, gc_ptr(Object) val) { m_setter(parent, val); }

  gc_ptr(Object) GetFieldValue(gc_ptr(Object) parent) { return m_getter(parent); }

  void SerializeField(gc_ptr(IPdxWriter) w, gc_ptr(Object) value) {
    switch (m_fieldType) {
      case FieldType::BOOLEAN:
        w->WriteBoolean(m_fieldName, (bool)value);
        break;
      case FieldType::BYTE:
        w->WriteByte(m_fieldName, (SByte)value);
        break;
      case FieldType::CHAR:
        w->WriteChar(m_fieldName, (Char)value);
        break;
      case FieldType::SHORT:
        w->WriteShort(m_fieldName, (short)value);
        break;
      case FieldType::USHORT:
        w->WriteShort(m_fieldName, (short)(UInt16)value);
        break;
      case FieldType::INT:
        w->WriteInt(m_fieldName, (int)value);
        break;
      case FieldType::UINT:
        w->WriteInt(m_fieldName, (int)(UInt32)value);
        break;
      case FieldType::LONG:
        w->WriteLong(m_fieldName, (Int64)value);
        break;
      case FieldType::ULONG:
        w->WriteLong(m_fieldName, (long)(UInt64)value);
        break;
      case FieldType::FLOAT:
        w->WriteFloat(m_fieldName, (float)value);
        break;
      case FieldType::DOUBLE:
        w->WriteDouble(m_fieldName, (double)value);
        break;
      case FieldType::DATE:
        w->WriteDate(m_fieldName, (DateTime)value);
        break;
      case FieldType::STRING:
        w->WriteString(m_fieldName, (gc_ptr(String))value);
        break;
      case FieldType::OBJECT:
        w->WriteObject(m_fieldName, value);
        break;
      case FieldType::BOOLEAN_ARRAY:
        w->WriteBooleanArray(m_fieldName, (gc_ptr(array<bool>))value);
        break;
      case FieldType::CHAR_ARRAY:
        w->WriteCharArray(m_fieldName, (gc_ptr(array<Char>))value);
        break;
      case FieldType::BYTE_ARRAY:
        w->WriteByteArray(m_fieldName, (gc_ptr(array<Byte>))value);
        break;
      case FieldType::SBYTE_ARRAY:
        w->WriteByteArray(m_fieldName, (gc_ptr(array<Byte>))value);
        break;
      case FieldType::SHORT_ARRAY:
        w->WriteShortArray(m_fieldName, (gc_ptr(array<Int16>))value);
        break;
      case FieldType::USHORT_ARRAY:
        w->WriteShortArray(m_fieldName, (gc_ptr(array<Int16>))(gc_ptr(array<UInt16>))value);
        break;
      case FieldType::INT_ARRAY:
        w->WriteIntArray(m_fieldName, (gc_ptr(array<Int32>))value);
        break;
      case FieldType::UINT_ARRAY:
        w->WriteIntArray(m_fieldName, (gc_ptr(array<Int32>))(gc_ptr(array<UInt32>))value);
        break;
      case FieldType::LONG_ARRAY:
        w->WriteLongArray(m_fieldName, (gc_ptr(array<Int64>))value);
        break;
      case FieldType::ULONG_ARRAY:
        w->WriteLongArray(m_fieldName, (gc_ptr(array<Int64>))(gc_ptr(array<UInt64>))value);
        break;
      case FieldType::FLOAT_ARRAY:
        w->WriteFloatArray(m_fieldName, (gc_ptr(array<float>))value);
        break;
      case FieldType::DOUBLE_ARRAY:
        w->WriteDoubleArray(m_fieldName, (gc_ptr(array<double>))value);
        break;
      case FieldType::STRING_ARRAY:
        w->WriteStringArray(m_fieldName, (gc_ptr(array<gc_ptr(String)>))value);
        break;
      case FieldType::OBJECT_ARRAY:
        w->WriteObjectArray(m_fieldName, safe_cast<gc_ptr(System::Collections::Generic::List<gc_ptr(Object)>)>(value));
        break;
      case FieldType::ARRAY_OF_BYTE_ARRAYS:
        w->WriteArrayOfByteArrays(m_fieldName, (array<gc_ptr(array<Byte>)> ^) value);
        break;
      case FieldType::GUID:
        w->WriteByteArray(m_fieldName, ((System::Guid)value).ToByteArray());
        break;
      case FieldType::DECIMAL:
        w->WriteIntArray(m_fieldName, Decimal::GetBits((System::Decimal)value));
        break;
      default:
        throw gcnew IllegalStateException("Not found FieldType: " + m_fieldType.ToString());
    }
  }

  gc_ptr(Object) DeserializeField(gc_ptr(IPdxReader) r) {
    switch (m_fieldType) {
      case FieldType::BOOLEAN:
        return r->ReadBoolean(m_fieldName);
        break;
      case FieldType::BYTE:
        return r->ReadByte(m_fieldName);
        break;
      case FieldType::SBYTE:
        return r->ReadByte(m_fieldName);
        break;
      case FieldType::CHAR:
        return r->ReadChar(m_fieldName);
        break;
      case FieldType::SHORT:
        return r->ReadShort(m_fieldName);
        break;
      case FieldType::USHORT:
        return (USHORT)r->ReadShort(m_fieldName);
        break;
      case FieldType::INT:
        return r->ReadInt(m_fieldName);
        break;
      case FieldType::UINT:
        return (UINT)r->ReadInt(m_fieldName);
        break;
      case FieldType::LONG:
        return r->ReadLong(m_fieldName);
        break;
      case FieldType::ULONG:
        return (ULONGLONG)r->ReadLong(m_fieldName);
        break;
      case FieldType::FLOAT:
        return r->ReadFloat(m_fieldName);
        break;
      case FieldType::DOUBLE:
        return r->ReadDouble(m_fieldName);
        break;
      case FieldType::DATE:
        return r->ReadDate(m_fieldName);
        break;
      case FieldType::STRING:
        return r->ReadString(m_fieldName);
        break;
      case FieldType::OBJECT:
        return r->ReadObject(m_fieldName);
        break;
      case FieldType::BOOLEAN_ARRAY:
        return r->ReadBooleanArray(m_fieldName);
        break;
      case FieldType::CHAR_ARRAY:
        return r->ReadCharArray(m_fieldName);
        break;
      case FieldType::BYTE_ARRAY:
        return r->ReadByteArray(m_fieldName);
        break;
      case FieldType::SBYTE_ARRAY:
        return (gc_ptr(array<SByte>))r->ReadByteArray(m_fieldName);
        break;
      case FieldType::SHORT_ARRAY:
        return r->ReadShortArray(m_fieldName);
        break;
      case FieldType::USHORT_ARRAY:
        return (gc_ptr(array<USHORT>))r->ReadShortArray(m_fieldName);
        break;
      case FieldType::INT_ARRAY:
        return r->ReadIntArray(m_fieldName);
        break;
      case FieldType::UINT_ARRAY:
        return (gc_ptr(array<UINT>))r->ReadIntArray(m_fieldName);
        break;
      case FieldType::LONG_ARRAY:
        return r->ReadLongArray(m_fieldName);
        break;
      case FieldType::ULONG_ARRAY:
        return (gc_ptr(array<ULONGLONG>))r->ReadLongArray(m_fieldName);
        break;
      case FieldType::FLOAT_ARRAY:
        return r->ReadFloatArray(m_fieldName);
        break;
      case FieldType::DOUBLE_ARRAY:
        return r->ReadDoubleArray(m_fieldName);
        break;
      case FieldType::STRING_ARRAY:
        return r->ReadStringArray(m_fieldName);
        break;
      case FieldType::OBJECT_ARRAY:
        return r->ReadObjectArray(m_fieldName);
        break;
      case FieldType::ARRAY_OF_BYTE_ARRAYS:
        return r->ReadArrayOfByteArrays(m_fieldName);
        break;
      case FieldType::GUID:
        return gcnew Guid(r->ReadByteArray(m_fieldName));
        break;
      case FieldType::DECIMAL:
        return gcnew Decimal(r->ReadIntArray(m_fieldName));
        break;
      default:
        throw gcnew IllegalStateException("Not found FieldType: " + m_fieldType.ToString());
    }
    return nullptr;
  }
};

ReflectionBasedAutoSerializer::ReflectionBasedAutoSerializer() {
  gc_ptr(PdxIdentityFieldAttribute) pif = gcnew PdxIdentityFieldAttribute();
  PdxIdentityFieldAttributeType = pif->GetType();
  classNameVsFieldInfoWrapper = gcnew Dictionary<gc_ptr(String), gc_ptr(List<gc_ptr(FieldWrapper)>)>();
}

bool ReflectionBasedAutoSerializer::ToData(gc_ptr(Object) o, gc_ptr(IPdxWriter) writer) {
  serializeFields(o, writer);
  return true;
}

gc_ptr(Object) ReflectionBasedAutoSerializer::FromData(gc_ptr(String) o, gc_ptr(IPdxReader) reader) {
  return deserializeFields(o, reader);
}

void ReflectionBasedAutoSerializer::serializeFields(gc_ptr(Object) o, gc_ptr(IPdxWriter) writer) {
  gc_ptr(Type) ty = o->GetType();
  FOR_EACH (gc_ptr(FieldWrapper) fi in GetFields(o->GetType())) {
    gc_ptr(Object) originalValue = fi->GetFieldValue(o);

    // Transform the value, in case the base class is overridden
    originalValue = WriteTransform(fi->FI, fi->FType, originalValue);

    fi->SerializeField(writer, originalValue);

    if (fi->isIdentityField) {
      writer->MarkIdentityField(fi->FieldName);
    }
  }
}

gc_ptr(Object) ReflectionBasedAutoSerializer::deserializeFields(gc_ptr(String) className, gc_ptr(IPdxReader) reader) {
  gc_ptr(Object) object = this->CreateObject(className, reader->Cache);

  FOR_EACH (gc_ptr(FieldWrapper) fi in GetFields(object->GetType())) {
    gc_ptr(Object) serializeValue = fi->DeserializeField(reader);

    // Transform the value, in case the base class is overridden
    serializeValue = ReadTransform(fi->FI, fi->FType, serializeValue);

    fi->SetFieldValue(object, serializeValue);
  }

  return object;
}

gc_ptr(Object) ReflectionBasedAutoSerializer::CreateObject(gc_ptr(String) className, gc_ptr(Cache) cache) {
  return cache->TypeRegistry->CreateObject(className);
}

bool ReflectionBasedAutoSerializer::IsPdxIdentityField(gc_ptr(FieldInfo) fi) {
  gc_ptr(array<gc_ptr(Object)>) cAttr = fi->GetCustomAttributes(PdxIdentityFieldAttributeType, true);
  if (cAttr != nullptr && cAttr->Length > 0) {
    gc_ptr(PdxIdentityFieldAttribute) pifa = (gc_ptr(PdxIdentityFieldAttribute))(cAttr[0]);
    return true;
  }
  return false;
}

gc_ptr(List<gc_ptr(FieldWrapper)>) ReflectionBasedAutoSerializer::GetFields(gc_ptr(Type) domaimType) {
  gc_ptr(List<gc_ptr(FieldWrapper)>) retVal = nullptr;

  gc_ptr(String) className = domaimType->FullName;
  System::Collections::Generic::Dictionary<gc_ptr(String), gc_ptr(List<gc_ptr(FieldWrapper)>)> ^ tmp =
      classNameVsFieldInfoWrapper;
  tmp->TryGetValue(className, retVal);
  if (retVal != nullptr) return retVal;
  msclr::lock lockInstance(classNameVsFieldInfoWrapper);
  {
    tmp = classNameVsFieldInfoWrapper;
    tmp->TryGetValue(className, retVal);
    if (retVal != nullptr) return retVal;

    gc_ptr(List<gc_ptr(FieldWrapper)>) collectFields = gcnew List<gc_ptr(FieldWrapper)>();
    while (domaimType != nullptr) {
      FOR_EACH (gc_ptr(FieldInfo) fi in domaimType->GetFields(BindingFlags::Public | BindingFlags::NonPublic |
                                                              BindingFlags::Instance | BindingFlags::DeclaredOnly)) {
        if (!fi->IsNotSerialized && !fi->IsStatic && !fi->IsLiteral && !fi->IsInitOnly) {
          // to ignore the fild
          if (IsFieldIncluded(fi, domaimType)) {
            // This are all hooks which app can implement

            gc_ptr(String) fieldName = GetFieldName(fi, domaimType);
            bool isIdentityField = IsIdentityField(fi, domaimType);
            FieldType ft = GetFieldType(fi, domaimType);

            gc_ptr(FieldWrapper) fw = gcnew FieldWrapper(fi, fieldName, isIdentityField, ft);

            collectFields->Add(fw);
          }
        }
      }
      domaimType = domaimType->BaseType;
    }
    tmp = gcnew System::Collections::Generic::Dictionary<gc_ptr(String), gc_ptr(List<gc_ptr(FieldWrapper)>)>(
        classNameVsFieldInfoWrapper);
    tmp->Add(className, collectFields);
    classNameVsFieldInfoWrapper = tmp;

    return collectFields;
  }
}

gc_ptr(String) ReflectionBasedAutoSerializer::GetFieldName(gc_ptr(FieldInfo) fi, gc_ptr(Type) type) { return fi->Name; }

bool ReflectionBasedAutoSerializer::IsIdentityField(gc_ptr(FieldInfo) fi, gc_ptr(Type) type) {
  return IsPdxIdentityField(fi);
}

FieldType ReflectionBasedAutoSerializer::GetFieldType(gc_ptr(FieldInfo) fi, gc_ptr(Type) type) {
  return getPdxFieldType(fi->FieldType);
}

bool ReflectionBasedAutoSerializer::IsFieldIncluded(gc_ptr(FieldInfo) fi, gc_ptr(Type) type) { return true; }

gc_ptr(Object) ReflectionBasedAutoSerializer::WriteTransform(gc_ptr(FieldInfo) fi, gc_ptr(Type) type,
                                                             gc_ptr(Object) originalValue) {
  return originalValue;
}

gc_ptr(Object) ReflectionBasedAutoSerializer::ReadTransform(gc_ptr(FieldInfo) fi, gc_ptr(Type) type,
                                                            gc_ptr(Object) serializeValue) {
  return serializeValue;
}

FieldType ReflectionBasedAutoSerializer::getPdxFieldType(gc_ptr(Type) type) {
  if (type->Equals(Internal::DotNetTypes::IntType)) {
    return FieldType::INT;
  } else if (type->Equals(Internal::DotNetTypes::UIntType)) {
    return FieldType::UINT;
  } else if (type->Equals(Internal::DotNetTypes::StringType)) {
    return FieldType::STRING;
  } else if (type->Equals(Internal::DotNetTypes::BooleanType)) {
    return FieldType::BOOLEAN;
  } else if (type->Equals(Internal::DotNetTypes::FloatType)) {
    return FieldType::FLOAT;
  } else if (type->Equals(Internal::DotNetTypes::DoubleType)) {
    return FieldType::DOUBLE;
  } else if (type->Equals(Internal::DotNetTypes::CharType)) {
    return FieldType::CHAR;
  } else if (type->Equals(Internal::DotNetTypes::SByteType)) {
    return FieldType::BYTE;
  } else if (type->Equals(Internal::DotNetTypes::ShortType)) {
    return FieldType::SHORT;
  } else if (type->Equals(Internal::DotNetTypes::UShortType)) {
    return FieldType::USHORT;
  } else if (type->Equals(Internal::DotNetTypes::LongType)) {
    return FieldType::LONG;
  } else if (type->Equals(Internal::DotNetTypes::ULongType)) {
    return FieldType::ULONG;
  } else if (type->Equals(Internal::DotNetTypes::ByteArrayType)) {
    return FieldType::BYTE_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::SByteArrayType)) {
    return FieldType::SBYTE_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::DoubleArrayType)) {
    return FieldType::DOUBLE_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::FloatArrayType)) {
    return FieldType::FLOAT_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::ShortArrayType)) {
    return FieldType::SHORT_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::UShortArrayType)) {
    return FieldType::USHORT_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::IntArrayType)) {
    return FieldType::INT_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::UIntArrayType)) {
    return FieldType::UINT_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::LongArrayType)) {
    return FieldType::LONG_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::ULongArrayType)) {
    return FieldType::ULONG_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::BoolArrayType)) {
    return FieldType::BOOLEAN_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::CharArrayType)) {
    return FieldType::CHAR_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::StringArrayType)) {
    return FieldType::STRING_ARRAY;
  } else if (type->Equals(Internal::DotNetTypes::DateType)) {
    return FieldType::DATE;
  } else if (type->Equals(Internal::DotNetTypes::ByteArrayOfArrayType)) {
    return FieldType::ARRAY_OF_BYTE_ARRAYS;
  } else if (type->Equals(Internal::DotNetTypes::GuidType)) {
    return FieldType::GUID;
  } else if (type->Equals(Internal::DotNetTypes::DecimalType)) {
    return FieldType::DECIMAL;
  }
  /*else if(type->Equals(Internal::DotNetTypes::ObjectArrayType))
  {
    //Giving more preference to arraylist instead of Object[] in java side
    //return this->WriteObjectArray(fieldName,
  safe_cast<gc_ptr(System::Collections::Generic::List<gc_ptr(Object)>)>(fieldValue)); return FieldType::OBJECT_ARRAY;
  }*/
  else {
    return FieldType::OBJECT;
  }
}
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
