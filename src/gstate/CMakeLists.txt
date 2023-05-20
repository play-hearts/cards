add_library(gstate_lib STATIC
    GameBehavior.cpp
    GState.cpp
    PlayerVoids.cpp
    ScoreResult.cpp
    Trick.cpp
    utils.cpp
)

target_include_directories(gstate_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(WiseEnum REQUIRED)

target_link_libraries(gstate_lib
    cards_lib
    math_lib
    prim_lib
    stats_lib
    fmt::fmt
    WiseEnum::wise_enum
    )

add_subdirectory(tests)
