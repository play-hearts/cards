
add_library(math_lib STATIC
    combinatorics.cpp
    math.cpp
    random.cpp
)

target_include_directories(math_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
