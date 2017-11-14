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
cmake_minimum_required(VERSION 3.4)
project(nativeclient.tests.clicache)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages.config ${CMAKE_CURRENT_BINARY_DIR}/packages.config COPYONLY)

#set (NUNIT "C:\\Program Files (x86)\\NUnit 2.6.4")

if (64 EQUAL ${BUILD_BITS})
  set (NUNIT_CONSOLE "nunit3-console.exe")
else()
  set (NUNIT_CONSOLE "nunit3-console.exe -x86")
endif()

# Set the .NET Target Framework (Note: This should match the build for Apache.Geode.)
set (DOTNET_TARGET_FRAMEWORK_VERSION "v4.5.2")

foreach(var CMAKE_CURRENT_SOURCE_DIR CMAKE_SOURCE_DIR CMAKE_BINARY_DIR CMAKE_CURRENT_BINARY_DIR)
  file(TO_NATIVE_PATH ${${var}} ${var}_NATIVE)
endforeach()

file(GLOB_RECURSE CSPROJECTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.csproj.in")
if(EXISTS "${STRONG_NAME_KEY}")
  set(STRONG_NAME_KEY_ENABLED true)
endif()

configure_file(UnitTests.csproj.in ${CMAKE_CURRENT_BINARY_DIR}/UnitTests.csproj)
include_external_msproject(
  ${PROJECT_NAME}  ${CMAKE_CURRENT_BINARY_DIR}/UnitTests.csproj
  TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER test/cliTests)
add_dependencies(${PROJECT_NAME} nuget-restore)

# Function to resolve both config and generate stage variables.
macro(generate_config INPUT TEMP OUTPUT)
    configure_file(${INPUT} ${TEMP})
    file(GENERATE OUTPUT ${OUTPUT}
      INPUT ${TEMP}
      CONDITION 1
    )
endmacro()

foreach( lib ssl xerces-c sqlite )
  get_target_property(runtime_path ${lib} INTERFACE_RUNTIME_DIR)
  set(PATH ${PATH} ${runtime_path})
endforeach()

enable_testing()
include(NUnit)
nunit_discover_tests(${PROJECT_NAME}
  TEST_ASSEMBLY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/UnitTests.dll"
)

#TODO make work in vstest_discover_tests
set_directory_properties(PROPERTIES
  TEST_INCLUDE_FILE "${PROJECT_NAME}_include.cmake"
)

# Label any flaky tests here
#set_property(TEST ThinClientCqTestsN PROPERTY LABELS FLAKY)
#set_property(TEST ThinClientQueryTestsN PROPERTY LABELS FLAKY)
#
## Label any tests that always fail here
#set_property(TEST OverflowTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientDeltaTestN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientDurableTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientHARegionTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientPdxTests PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientRegionTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientSecurityAuthTestsMUN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientSecurityAuthTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientSecurityAuthTestsMUN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientSecurityAuthzTestsMUN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientSecurityAuthzTestsN PROPERTY LABELS OMITTED)
#set_property(TEST ThinClientStatisticTestsN PROPERTY LABELS OMITTED)

add_custom_target(run-stable-clicache-integration-tests
  COMMAND ctest -C $<CONFIGURATION> -L STABLE
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  USES_TERMINAL)
add_dependencies(run-integration-tests run-stable-clicache-integration-tests)
set_target_properties(run-stable-clicache-integration-tests PROPERTIES EXCLUDE_FROM_ALL TRUE EXCLUDE_FROM_DEFAULT_BUILD TRUE)
