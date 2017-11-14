set(prefix "${TEST_PREFIX}")
set(suffix "${TEST_SUFFIX}")
set(extra_args ${TEST_EXTRA_ARGS})
set(script)
set(tests)

function(add_command NAME)
  set(_args "")
  foreach(_arg ${ARGN})
    if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
      set(_args "${_args} [==[${_arg}]==]")
    else()
      set(_args "${_args} ${_arg}")
    endif()
  endforeach()
  set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# Run test executable to get list of available tests
if(NOT EXISTS "${TEST_ASSEMBLY}")
  message(FATAL_ERROR
    "Specified test executable '${TEST_ASSEMBLY}' does not exist"
  )
endif()

execute_process(
  COMMAND ${TEST_NUNIT_EXECUTABLE}
	  "${TEST_ASSEMBLY}"
    "--explore:tests.out;format=cases"
    --where "_SKIPREASON!~//"
  OUTPUT_VARIABLE output
  RESULT_VARIABLE result
)
if(NOT ${result} EQUAL 0)
  message(FATAL_ERROR
    "Error running test executable '${TEST_ASSEMBLY}':\n"
    "  Result: ${result}\n"
    "  Output: ${output}\n"
  )
endif()

file(STRINGS tests.out output)

# Parse output
foreach(test ${output})
  message("Discovered NUnit test: ${test}")
  add_command(add_test
    "${test}"
    ${TEST_NUNIT_EXECUTABLE}
    "${TEST_ASSEMBLY}"
    --test "${test}"
    --noheader
    ${extra_args}
  )

  add_command(set_tests_properties "${test}" PROPERTIES
     ENVIRONMENT "PATH=$<SHELL_PATH:${CMAKE_CURRENT_BINARY_DIR}>\$<CONFIG>;%PATH%"
  )

 list(APPEND tests ${test})
endforeach()

add_command(set ${TEST_LIST} ${tests})

# Write CTest script
file(WRITE "${CTEST_FILE}" "${script}")
