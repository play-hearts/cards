create_test(Card
    DEPENDS
    cards_lib
)

create_test(CardSet
    DEPENDS
    cards_lib
)

create_test(Deal
    DEPENDS
    cards_lib
)

create_test(FourHands
    DEPENDS
    cards_lib
)

add_custom_target(run_all_cards_tests)

add_dependencies(run_all_cards_tests
    run_Card_test
    run_CardSet_test
    run_Deal_test
    run_FourHands_test
)
