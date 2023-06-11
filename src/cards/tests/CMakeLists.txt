create_test(Card
    DEPENDS
    math_lib
    cards_lib
)

create_test(CardSet
    DEPENDS
    math_lib
    cards_lib
)

create_test(Deal
    DEPENDS
    math_lib
    cards_lib
)

create_test(FourHands
    DEPENDS
    math_lib
    cards_lib
)

add_custom_target(run_all_cards_tests)

add_dependencies(run_all_cards_tests
    run_Card_test
    run_CardSet_test
    run_Deal_test
    run_FourHands_test
)
