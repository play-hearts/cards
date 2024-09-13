
include_directories(${PROJECT_SOURCE_DIR})
add_library(prim_lib
    split.cpp
)

target_include_directories(prim_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(prim_lib
    fmt::fmt
)

# add_library(prim_lib INTERFACE)
# target_link_libraries(prim_lib INTERFACE prim_obj_lib $<TARGET_OBJECTS:prim_obj_lib>)
