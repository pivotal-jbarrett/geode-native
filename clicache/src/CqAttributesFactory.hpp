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
#include "begin_native.hpp"
#include <geode/CqAttributesFactory.hpp>
#include "end_native.hpp"

#include "impl/SafeConvert.hpp"
#include "CqAttributes.hpp"
#include "native_unique_ptr.hpp"

using namespace System;
using namespace System::Collections::Generic;

namespace Apache {
namespace Geode {
namespace Client {
namespace native = apache::geode::client;

GENERIC(class TKey, class TResult)
interface class ICqListener;

/// <summary>
/// Creates instances of <c>CqAttributes</c>.
/// </summary>
/// <seealso cref="CqAttributes" />
GENERIC(class TKey, class TResult)
PUBLIC ref class CqAttributesFactory sealed {
 public:
  /// <summary>
  /// Creates a new instance of <c>CqAttributesFactory</c> ready
  /// to create a <c>CqAttributes</c> with default settings.
  /// </summary>
  inline CqAttributesFactory() {
    m_nativeptr = gcnew native_unique_ptr<native::CqAttributesFactory>(std::make_unique<native::CqAttributesFactory>());
  }

  inline CqAttributesFactory(gc_ptr(Client::CqAttributes<TKey, TResult>) cqAttributes) {
    m_nativeptr = gcnew native_unique_ptr<native::CqAttributesFactory>(
        std::make_unique<native::CqAttributesFactory>(cqAttributes->GetNative()));
  }

  // ATTRIBUTES

  /// <summary>
  /// add a cqListener
  /// </summary>
  void AddCqListener(gc_ptr(Client::ICqListener<TKey, TResult>) cqListener);

  /// <summary>
  /// Initialize with an array of listeners
  /// </summary>
  void InitCqListeners(array<gc_ptr(Client::ICqListener<TKey, TResult>)> ^ cqListeners);

  // FACTORY METHOD

  /// <summary>
  /// Creates a <c>CqAttributes</c> with the current settings.
  /// </summary>
  gc_ptr(Client::CqAttributes<TKey, TResult>) Create();

 private:
  gc_ptr(native_unique_ptr<native::CqAttributesFactory>) m_nativeptr;
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
