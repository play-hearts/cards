
include_directories(${PROJECT_SOURCE_DIR})
add_library(cards_lib
    Card.cpp
    CardHands.cpp
    CardSet.cpp
    Deal.cpp
    FourHands.cpp
    utils.cpp
)

target_link_libraries(cards_lib
    math_lib
    prim_lib
)

target_include_directories(cards_lib
  PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#add_library(cards_lib INTERFACE)
#target_link_libraries(cards_lib INTERFACE cards_obj_lib $<TARGET_OBJECTS:cards_obj_lib>)

add_subdirectory(tests)

wasm_module(cards
    DEPENDS
    cards_lib
    math_lib
    prim_lib
    )
