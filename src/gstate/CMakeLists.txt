add_library(gstate_lib
    GameBehavior.cpp
    GameVariant.cpp
    GState.cpp
    PlayerVoids.cpp
    ScoreResult.cpp
    Trick.cpp
)

target_include_directories(gstate_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(gstate_lib
    cards_lib
    math_lib
    prim_lib
    stats_lib
    fmt::fmt
    magic_enum
    )

# add_library(gstate_lib INTERFACE)
# target_link_libraries(gstate_lib INTERFACE gstate_obj_lib $<TARGET_OBJECTS:gstate_obj_lib>)


wasm_module(gstate
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

add_subdirectory(tests)
