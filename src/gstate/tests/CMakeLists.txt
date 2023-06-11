create_test(GameBehavior
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

create_test(GameOutcome
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

create_test(GState
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

create_test(ScoreResult
    DEPENDS
    gstate_lib
    cards_lib
    math_lib
    prim_lib
)

add_custom_target(run_all_gstate_tests)
add_dependencies(run_all_gstate_tests
    run_GameBehavior_test
    run_GameOutcome_test
    run_GState_test
    run_ScoreResult_test
)
