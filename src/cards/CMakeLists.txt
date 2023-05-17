
include_directories(${PROJECT_SOURCE_DIR})
add_library(cards_lib STATIC
    Card.cpp
    CardHands.cpp
    CardSet.cpp
    Deal.cpp
    FourHands.cpp
)

target_link_libraries(cards_lib
    math_lib
    prim_lib
)

target_include_directories(cards_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)
