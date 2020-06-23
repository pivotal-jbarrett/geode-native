# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project( sqlite LANGUAGES NONE )

include(ExternalProject)
ExternalProject_Add( ${PROJECT_NAME}-extern
  GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation
  GIT_SHALLOW TRUE
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=$<CONFIG>
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DSQLITE_ENABLE_JSON1=OFF
)

ExternalProject_Get_Property( ${PROJECT_NAME}-extern SOURCE_DIR )
ExternalProject_Get_Property( ${PROJECT_NAME}-extern INSTALL_DIR )

add_library(SQLite_sqlite3 INTERFACE)
target_include_directories(SQLite_sqlite3 SYSTEM INTERFACE
  $<BUILD_INTERFACE:${INSTALL_DIR}/include>
)
target_link_libraries(SQLite_sqlite3 INTERFACE
  ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sqlite3$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(SQLite_sqlite3 ${PROJECT_NAME}-extern)

add_library(SQLite::sqlite3 ALIAS SQLite_sqlite3)
