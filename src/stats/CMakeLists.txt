add_library(stats_lib STATIC
    RunningStats.cpp
)

target_include_directories(stats_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(stats_lib
    fmt::fmt
    )
