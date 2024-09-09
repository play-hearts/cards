
add_library(math_obj_lib OBJECT
    combinatorics.cpp
    elo.cpp
    math.cpp
    random.cpp
)

target_include_directories(math_obj_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(math_obj_lib prim_lib)

add_library(math_lib INTERFACE)
target_link_libraries(math_lib INTERFACE math_obj_lib $<TARGET_OBJECTS:math_obj_lib>)


wasm_module(math
    DEPENDS
    math_lib
)

add_subdirectory(tests)
