
add_library(math_lib OBJECT
    combinatorics.cpp
    elo.cpp
    math.cpp
    random.cpp
)

target_include_directories(math_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(math_lib prim_lib)

wasm_module(math
    DEPENDS
    math_lib
)

add_subdirectory(tests)
