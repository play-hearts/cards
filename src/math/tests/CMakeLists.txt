create_test(Bits
    DEPENDS
    math_lib
    gtest
)

create_test(combinatorics
    DEPENDS
    math_lib
    gtest
)

create_test(elo
    DEPENDS
    math_lib
    gtest
)

create_test(random
    DEPENDS
    math_lib
    gtest
)

create_test(Int128
    DEPENDS
    math_lib
    gtest
)

add_custom_target(run_all_math_tests)
add_dependencies(run_all_math_tests
    run_Bits_test
    run_combinatorics_test
    run_elo_test
    run_random_test
    run_Int128_test
)
