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

cmake_minimum_required(VERSION 3.9)
project(Apache.Geode.IntegrationTests CSharp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages.config ${CMAKE_CURRENT_BINARY_DIR}/packages.config COPYONLY)


if (64 EQUAL ${BUILD_BITS})
  set (NUNIT_CONSOLE "nunit3-console.exe")
else()
  set (NUNIT_CONSOLE "nunit3-console.exe -x86")
endif()

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cs")
add_library( ${PROJECT_NAME} SHARED ${SOURCES}

# TODO make these their own assembly
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/DeltaTestImpl.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/TestObject1.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/DeltaEx.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/PortfolioPdx.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/PositionPdx.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/Portfolio.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/Position.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/NewTestObject/SimpleCacheListener.cs
  ${CMAKE_SOURCE_DIR}/tests/cli/QueryHelper/QueryHelperN.cs
)

target_link_libraries(${PROJECT_NAME}
  Apache.Geode
  QueryWrapper
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VS_GLOBAL_ROOTNAMESPACE ${PROJECT_NAME}
  VS_GLOBAL_TestProjectType UnitTest
  VS_GLOBAL_PROJECT_TYPES "{3AC096D0-A1C2-E12C-1390-A8335801FDAB};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}"
  VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.5.2"
  VS_DOTNET_REFERENCES "System;System.Xml;System.Data;System.Management"
#  VS_DOTNET_REFERENCE_Microsoft.VisualStudio.TestPlatform.TestFramework "${CMAKE_BINARY_DIR}/packages/Microsoft.VisualStudio.TestPlatform.14.0.0.1/lib/net20/Microsoft.VisualStudio.TestPlatform.TestFramework.dll"
#  VS_DOTNET_REFERENCE_Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions "${CMAKE_BINARY_DIR}/packages/Microsoft.VisualStudio.TestPlatform.14.0.0.1/lib/net20/Microsoft.VisualStudio.TestPlatform.TestFramework.Extensions.dll"
  VS_DOTNET_REFERENCE_nunit.framework "${CMAKE_BINARY_DIR}/packages/NUnit.3.9.0/lib/net45/nunit.framework.dll"
)

if(NOT "${STRONG_NAME_KEY}" STREQUAL "")
  set_target_properties( ${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_SignAssembly "true"
    VS_GLOBAL_AssemblyOriginatorKeyFile ${STRONG_NAME_KEY}
  )
endif()

target_compile_options(${PROJECT_NAME} PRIVATE "/langversion:6")

add_dependencies(${PROJECT_NAME}
  DUnitFramework
  FwkClient
  PdxClassLibrary
  PdxVersion1Lib
  PdxVersion2Lib
#  QueryWrapper
  SecurityUtil
  SQLiteCLI
  nuget-restore
)

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
nunit_discover_tests(${PROJECT_NAME})

#TODO make work in vstest_discover_tests
set_directory_properties(PROPERTIES
  TEST_INCLUDE_FILE "${PROJECT_NAME}_include.cmake"
)

#set_tests_properties(${${PROJECT_NAME}_TESTS} PROPERTIES
#  ENVIRONMENT "PATH=$<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}>\$<CONFIG>;%PATH%"
#)

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
