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

// clang-format off

// C++/CLI for each
#define FOR_EACH for each

// clang-format on

// C++/CLI generic<>
#define GENERIC(...) generic<__VA_ARGS__>

// C++/CLI where T : ...
#define WHERE(T, ...) where T : __VA_ARGS__

// C++/CLI safe statement
#define CLI(...) __VA_ARGS__

// public
#define PUBLIC public

// Object ^
#define gc_ptr(...) __VA_ARGS__ ^

#define BEGIN_NATIVE \
  do {               \
  try

#define END_NATIVE                                        \
  catch (const apache::geode::client::Exception &ex) {    \
    throw Apache::Geode::Client::GeodeException::Get(ex); \
  }                                                       \
  finally { GC::KeepAlive(this); }                        \
  }                                                       \
  while (false)

/*
 * Type<X ^, Y ^> ^ -> gc_ptr(Type<X ^, Y ^>) : \w+<.*\^, .*\^> \^
 *
 * ([\w:]+)\s*\^ -> gc_ptr($1)
 * ([\w:]+(?:<[^<>]+>))\s*\^ -> gc_ptr($1)
 */