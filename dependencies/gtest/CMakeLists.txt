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

project( gtest LANGUAGES NONE )

# Google Test now follows the Abseil Live at Head philosophy, so always fetch head

find_package(Threads REQUIRED)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
  set(CMAKE_ARGS ${CMAKE_ARGS} -Dgtest_force_shared_crt:BOOL=ON)
endif()

include(ExternalProject)
ExternalProject_Add( ${PROJECT_NAME}-extern
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_SHALLOW TRUE
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    -DCMAKE_BUILD_TYPE=$<CONFIG>
    -DINSTALL_GTEST=OFF
    ${CMAKE_ARGS}
)

ExternalProject_Get_Property( ${PROJECT_NAME}-extern SOURCE_DIR )
ExternalProject_Get_Property( ${PROJECT_NAME}-extern BINARY_DIR )

add_library(GTest_gtest INTERFACE)
target_include_directories(GTest_gtest SYSTEM INTERFACE
  $<BUILD_INTERFACE:${SOURCE_DIR}/googletest/include>
)
target_link_libraries(GTest_gtest INTERFACE
  ${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
  Threads::Threads
)
add_dependencies(GTest_gtest ${PROJECT_NAME}-extern)
add_library(GTest::gtest ALIAS GTest_gtest)

add_library(GTest_gtest_main INTERFACE)
target_link_libraries(GTest_gtest_main INTERFACE
  ${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
  GTest::gtest
)
add_dependencies(GTest_gtest_main ${PROJECT_NAME}-extern)
add_library(GTest::gtest_main ALIAS GTest_gtest_main)

add_library(GTest_gmock INTERFACE)
target_include_directories(GTest_gmock SYSTEM INTERFACE
  $<BUILD_INTERFACE:${SOURCE_DIR}/googlemock/include>
)
target_link_libraries(GTest_gmock INTERFACE
  ${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gmock$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
  Threads::Threads
)
add_dependencies(GTest_gmock ${PROJECT_NAME}-extern)
add_library(GTest::gmock ALIAS GTest_gmock)

add_library(GTest_gmock_main INTERFACE)
target_link_libraries(GTest_gmock_main INTERFACE
  ${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
  GTest::gmock
)
add_dependencies(GTest_gmock_main ${PROJECT_NAME}-extern)
add_library(GTest::gmock_main ALIAS GTest_gmock_main)
