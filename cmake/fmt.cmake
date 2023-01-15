include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 81f1cc74a776581cdef8659d176049d3aeb743c6
)

FetchContent_MakeAvailable(fmt)
