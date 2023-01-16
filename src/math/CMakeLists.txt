
add_library(math_lib STATIC
    combinatorics.cpp
    math.cpp
    random.cpp
)

target_include_directories(math_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(math_lib prim_lib)

add_subdirectory(tests)
