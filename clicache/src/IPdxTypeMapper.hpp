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

using namespace System;
using namespace System::Collections::Generic;
namespace Apache {
namespace Geode {
namespace Client {

/// <summary>
/// Application can implement this interface to map pdx type name to local type name.
/// Need to set this using <see cref="Serializable.SetPdxTypeMapper" />
/// </summary>
PUBLIC interface class IPdxTypeMapper {
 public:
  /// <summary>
  /// To map the local type name to pdx type
  /// <param name="localTypeName"> local type name </param>
  /// @return the pdx type name.
  /// </summary>
  gc_ptr(String) ToPdxTypeName(gc_ptr(String) localTypeName);

  /// <summary>
  /// To map the pdx type name to local type
  /// <param name="pdxTypeName"> pdx type name </param>
  /// @return the local type name.
  /// </summary>
  gc_ptr(String) FromPdxTypeName(gc_ptr(String) pdxTypeName);
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
