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

#ifdef CSTX_COMMENTED

#include "ManagedTransactionWriter.hpp"
//#include "../TransactionEvent.hpp"
#include "../Log.hpp"
#include "../ExceptionTypes.hpp"
//#include "../ITransactionWriter.hpp"
#include "ManagedString.hpp"

using namespace System;
using namespace System::Text;
using namespace System::Reflection;

// using namespace Apache::Geode::Client;

namespace apache {
namespace geode {
namespace client {

apache::geode::client::TransactionWriter* ManagedTransactionWriterGeneric::create(const char* assemblyPath,
                                                                                  const char* factoryFunctionName) {
  try {
    gc_ptr(String) mg_assemblyPath = Apache::Geode::Client::ManagedString::Get(assemblyPath);
    gc_ptr(String) mg_factoryFunctionName = Apache::Geode::Client::ManagedString::Get(factoryFunctionName);
    gc_ptr(String) mg_typeName = nullptr;

    gc_ptr(String) mg_genericKey = nullptr;
    gc_ptr(String) mg_genericVal = nullptr;

    System::Int32 dotIndx = -1;
    System::Int32 genericsOpenIndx = -1;
    System::Int32 genericsCloseIndx = -1;
    System::Int32 commaIndx = -1;

    if (mg_factoryFunctionName == nullptr || (dotIndx = mg_factoryFunctionName->LastIndexOf('.')) < 0) {
      std::string ex_str = "ManagedTransactionWriterGeneric: Factory function name '";
      ex_str += factoryFunctionName;
      ex_str += "' does not contain type name";
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }

    if ((genericsCloseIndx = mg_factoryFunctionName->LastIndexOf('>')) < 0) {
      std::string ex_str = "ManagedTransactionWriterGeneric: Factory function name '";
      ex_str += factoryFunctionName;
      ex_str += "' does not contain any generic type parameters";
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }

    if ((genericsOpenIndx = mg_factoryFunctionName->LastIndexOf('<')) < 0 || genericsOpenIndx > genericsCloseIndx) {
      std::string ex_str = "ManagedTransactionWriterGeneric: Factory function name '";
      ex_str += factoryFunctionName;
      ex_str += "' does not contain expected generic type parameters";
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }

    if ((commaIndx = mg_factoryFunctionName->LastIndexOf(',')) < 0 ||
        (commaIndx < genericsOpenIndx || commaIndx > genericsCloseIndx)) {
      std::string ex_str = "ManagedTransactionWriterGeneric: Factory function name '";
      ex_str += factoryFunctionName;
      ex_str += "' does not contain expected generic type parameter comma separator";
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }

    gc_ptr(StringBuilder) typeBuilder = gcnew StringBuilder(mg_factoryFunctionName->Substring(0, genericsOpenIndx));
    mg_typeName = typeBuilder->ToString();
    mg_genericKey = mg_factoryFunctionName->Substring(genericsOpenIndx + 1, commaIndx - genericsOpenIndx - 1);
    mg_genericKey = mg_genericKey->Trim();
    mg_genericVal = mg_factoryFunctionName->Substring(commaIndx + 1, genericsCloseIndx - commaIndx - 1);
    mg_genericVal = mg_genericVal->Trim();
    mg_factoryFunctionName = mg_factoryFunctionName->Substring(dotIndx + 1);

    Apache::Geode::Client::Log::Fine("Attempting to instantiate a [{0}<{1}, {2}>] via the [{3}] factory method.",
                                     mg_typeName, mg_genericKey, mg_genericVal, mg_factoryFunctionName);

    typeBuilder->Append("`2");
    mg_typeName = typeBuilder->ToString();

    gc_ptr(Assembly) assmb = nullptr;
    try {
      assmb = Assembly::Load(mg_assemblyPath);
    } catch (gc_ptr(System::Exception)) {
      assmb = nullptr;
    }
    if (assmb == nullptr) {
      std::string ex_str = "ManagedTransactionWriterGeneric: Could not load assembly: ";
      ex_str += assemblyPath;
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }

    Apache::Geode::Client::Log::Debug("Loading type: [{0}]", mg_typeName);

    gc_ptr(Type) typeInst = assmb->GetType(mg_typeName, false, true);

    if (typeInst != nullptr) {
      gc_ptr(array<gc_ptr(Type)>) types = gcnew array<gc_ptr(Type)>(2);
      types[0] = Type::GetType(mg_genericKey, false, true);
      types[1] = Type::GetType(mg_genericVal, false, true);

      if (types[0] == nullptr || types[1] == nullptr) {
        std::string ex_str = "ManagedTransactionWriterGeneric: Could not get both generic type argument instances";
        throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
      }

      typeInst = typeInst->MakeGenericType(types);
      Apache::Geode::Client::Log::Info("Loading function: [{0}]", mg_factoryFunctionName);

      gc_ptr(MethodInfo) mInfo = typeInst->GetMethod(
          mg_factoryFunctionName, BindingFlags::Public | BindingFlags::Static | BindingFlags::IgnoreCase);

      if (mInfo != nullptr) {
        gc_ptr(Object) userptr = nullptr;
        try {
          userptr = mInfo->Invoke(typeInst, nullptr);
        } catch (gc_ptr(System::Exception) ex) {
          Apache::Geode::Client::Log::Debug("{0}: {1}", ex->GetType()->Name, ex->Message);
          userptr = nullptr;
        }
        if (userptr == nullptr) {
          std::string ex_str =
              "ManagedTransactionWriterGeneric: Could not create "
              "object on invoking factory function [";
          ex_str += factoryFunctionName;
          ex_str += "] in assembly: ";
          ex_str += assemblyPath;
          throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
        }
        return new ManagedTransactionWriterGeneric(userptr);
      } else {
        std::string ex_str =
            "ManagedTransactionWriterGeneric: Could not load "
            "function with name [";
        ex_str += factoryFunctionName;
        ex_str += "] in assembly: ";
        ex_str += assemblyPath;
        throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
      }
    } else {
      Apache::Geode::Client::ManagedString typeName(mg_typeName);
      std::string ex_str = "ManagedTransactionWriterGeneric: Could not load type [";
      ex_str += typeName.CharPtr;
      ex_str += "] in assembly: ";
      ex_str += assemblyPath;
      throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
    }
  } catch (const apache::geode::client::Exception&) {
    throw;
  } catch (gc_ptr(System::Exception) ex) {
    Apache::Geode::Client::ManagedString mg_exStr(ex->ToString());
    std::string ex_str =
        "ManagedTransactionWriterGeneric: Got an exception while "
        "loading managed library: ";
    ex_str += marshal_as<std::string>(exStr);
    throw apache::geode::client::IllegalArgumentException(ex_str.c_str());
  }
  return NULL;
}
void ManagedTransactionWriterGeneric::beforeCommit(std::shared_ptr<apache::geode::client::TransactionEvent>& te) {
  try {
    Apache::Geode::Client::TransactionEvent mevent(te.get());
    m_managedptr->BeforeCommit(% mevent);
  } catch (gc_ptr(Apache::Geode::Client::GeodeException) ex) {
    ex->ThrowNative();
  } catch (gc_ptr(System::Exception) ex) {
    Apache::Geode::Client::GeodeException::ThrowNative(ex);
  }
}

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif
