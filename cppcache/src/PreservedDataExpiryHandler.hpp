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

#ifndef GEODE_PRESERVEDDATAEXPIRYHANDLER_H_
#define GEODE_PRESERVEDDATAEXPIRYHANDLER_H_

#include <ace/Event_Handler.h>

#include <geode/PdxSerializable.hpp>

#include "ExpiryTaskManager.hpp"
#include "PdxTypeRegistry.hpp"

namespace apache {
namespace geode {
namespace client {

/**
 * The task object which contains the handler which gets triggered
 * when a preserved data expires.
 */
class APACHE_GEODE_EXPORT PreservedDataExpiryHandler
    : public ACE_Event_Handler {
 public:
  PreservedDataExpiryHandler(std::shared_ptr<PdxTypeRegistry> pdxTypeRegistry,
                             std::shared_ptr<PdxSerializable> pdxSerializable);

  int handle_timeout(const ACE_Time_Value& current_time,
                     const void* arg) override;

  int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask) override;

 private:
  std::shared_ptr<PdxTypeRegistry> pdxTypeRegistry_;
  std::shared_ptr<PdxSerializable> pdxSerializable_;
};

}  // namespace client
}  // namespace geode
}  // namespace apache

#endif  // GEODE_PRESERVEDDATAEXPIRYHANDLER_H_
