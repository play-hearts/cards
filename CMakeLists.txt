cmake_minimum_required (VERSION 3.24.0)

project (Cards)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

add_compile_options(-pthread)
add_link_options(-pthread)

if (EMSCRIPTEN)
    add_compile_options("SHELL:-sNO_DISABLE_EXCEPTION_CATCHING")
    add_link_options("SHELL:-s WASM_BIGINT")
    add_link_options("SHELL:-lembind")
    add_link_options("SHELL:-s LLD_REPORT_UNDEFINED")
    add_link_options("SHELL:-s EXPORT_ES6=1")
    add_link_options("SHELL:-s MODULARIZE=1")
    add_link_options("SHELL:-s SINGLE_FILE=1")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
include(cmake/magic_enum.cmake)

include(cmake/create_test.cmake)
include(cmake/wasm_module.cmake)

include(cmake/fmt.cmake)
include(cmake/googletest.cmake)

enable_testing()
add_subdirectory(src)
