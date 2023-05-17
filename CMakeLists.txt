cmake_minimum_required (VERSION 3.24.0)

project (Cards)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

add_compile_options(-pthread)
add_link_options(-pthread)

if (EMSCRIPTEN)
    add_link_options(--bind)
    add_link_options("SHELL:-s LLD_REPORT_UNDEFINED")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/create_test.cmake)

include(cmake/fmt.cmake)
include(cmake/googletest.cmake)

enable_testing()
add_subdirectory(src)
