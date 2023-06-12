
include_directories(${PROJECT_SOURCE_DIR})
add_library(prim_lib OBJECT
    split.cpp
)

target_include_directories(prim_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(prim_lib
    fmt::fmt
)
