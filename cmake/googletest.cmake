include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        b1e9b6323a17002b8359fef338c57bc0437caf57
)

if(NOT googletest_POPULATED)
  FetchContent_MakeAvailable(googletest)
endif()
