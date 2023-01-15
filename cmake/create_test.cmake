add_custom_target(test)
add_custom_target(all_tests)

function(create_test name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(PARSE_ARGV 1 CREATE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}")

    add_executable(${name}_test ${name}.cpp)
    target_link_libraries(${name}_test gtest gtest_main ${CREATE_TEST_DEPENDS})
    add_dependencies(test ${name}_test)

    add_custom_target(run_${name}_test COMMAND ${name}_test)

    add_dependencies(all_tests run_${name}_test)
endfunction()
