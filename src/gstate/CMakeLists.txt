add_library(gstate_lib OBJECT
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

wasm_module(gstate
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

add_subdirectory(tests)
