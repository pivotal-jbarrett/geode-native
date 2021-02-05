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

//#include "../../../geode_includes.hpp"
//#include "../../../IResultCollector.hpp"
//#include "../../../ResultCollector.hpp"
#include "../IResultCollector.hpp"

using namespace System;
// using namespace System::Collections::Generic;
// using namespace Apache::Geode::Client;

namespace Apache {
namespace Geode {
namespace Client {

PUBLIC interface class ResultCollectorG {
 public:
  void AddResult(const gc_ptr(Object) result);
  void EndResults();
  void ClearResults();
};

GENERIC(class TResult)
PUBLIC ref class ResultCollectorGeneric : ResultCollectorG {
 private:
  gc_ptr(IResultCollector<TResult>) m_rscoll;

 public:
  void SetResultCollector(gc_ptr(IResultCollector<TResult>) rscoll) { m_rscoll = rscoll; }

  virtual void AddResult(const gc_ptr(Object) rs) {
    // std::shared_ptr<apache::geode::client::Cacheable> nativeptr(rs);
    // TResult grs =  TypeRegistry::GetManagedValueGeneric<TResult>( nativeptr);
    m_rscoll->AddResult(safe_cast<const TResult>(rs));
  }

  virtual void EndResults() { m_rscoll->EndResults(); }

  virtual void ClearResults() { m_rscoll->ClearResults(); }
};
}  // namespace Client
}  // namespace Geode
}  // namespace Apache
