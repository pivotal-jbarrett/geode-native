#pragma once

#ifndef GEODE_TXCLEANER_H_
#define GEODE_TXCLEANER_H_

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
/*
 * TxCleaner.hpp
 *
 *  Created on: Nov 13, 2015
 *      Author: sshcherbakov
 */

#include "CacheTransactionManagerImpl.hpp"
#include "TSSTXStateWrapper.hpp"
#include "TXState.hpp"

namespace apache {
namespace geode {
namespace client {

class TXCleaner {
 public:
  explicit TXCleaner(CacheTransactionManagerImpl* cacheTxMgr);
  ~TXCleaner();

  void clean();
  TXState* getTXState();

 private:
  TXState* m_txState;
  CacheTransactionManagerImpl* m_cacheTxMgr;

  TXCleaner& operator=(const TXCleaner& other);
  TXCleaner(const TXCleaner& other);
};
}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_TXCLEANER_H_
