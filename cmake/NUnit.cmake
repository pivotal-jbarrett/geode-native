function(nunit_discover_tests TARGET)
  cmake_parse_arguments(
    ""
	""
	"WORKING_DIRECTORY;TEST_ASSEMBLY"
    "EXTRA_ARGS;PROPERTIES"
    ${ARGN}
  )

  if(NOT _WORKING_DIRECTORY)
    set(_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endif()
  if(NOT _TEST_ASSEMBLY)
    set(_TEST_ASSEMBLY "$<TARGET_FILE:${TARGET}>")
  endif()

  set(ctest_include_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_include.cmake")
  set(ctest_tests_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake")

  add_custom_command(
    TARGET ${TARGET} POST_BUILD
    BYPRODUCTS "${ctest_tests_file}"
    COMMAND "${CMAKE_COMMAND}"
            -D "TEST_TARGET=${TARGET}"
            -D "TEST_NUNIT_EXECUTABLE=${_NUNIT_EXECUTABLE}"
            -D "TEST_ASSEMBLY=${_TEST_ASSEMBLY}"
            -D "TEST_EXTRA_ARGS=${_EXTRA_ARGS}"
            -D "TEST_WORKING_DIR=${_WORKING_DIRECTORY}"
            -D "CTEST_FILE=${ctest_tests_file}"
            -P "${_NUNIT_DISCOVER_TESTS_SCRIPT}"
    VERBATIM
  )

  file(WRITE "${ctest_include_file}"
    "if(EXISTS \"${ctest_tests_file}\")\n"
    "  include(\"${ctest_tests_file}\")\n"
    "else()\n"
    "  add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)\n"
    "endif()\n"
  )

  # Add discovered tests to directory TEST_INCLUDE_FILES
  # not working in 3.9?
  set_property(DIRECTORY
    APPEND PROPERTY TEST_INCLUDE_FILES "${ctest_include_file}"
  )

endfunction()

###############################################################################

set(_NUNIT_DISCOVER_TESTS_SCRIPT
  ${CMAKE_CURRENT_LIST_DIR}/NUnitsAddTests.cmake
)

find_program(_NUNIT_EXECUTABLE 
  NAMES nunit3-console
)
