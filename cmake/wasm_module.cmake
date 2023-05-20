add_custom_target(all_wasm_tests)

# This script defines build targets for create all of the artifacts needed for one
# loadable wasm module.
# For example, `wasm_module(hearts)` will define the build targets to generate these artifacts:

# 1. hearts_wasm.wasm        -- the webassembly executable module/library
# 2. hearts_wasm             -- the js wrapper that loads the wasm and exports an interface
# 3. hearts_wasm.worker.js   -- the support for multithreading

# The `wasm_module()` cmake function generates a loadable module much link a dynamic link library,
# that could be used by several different javascript applications, server side (node) or
# client side (browser). The `wasm_module()` function should be invoked in the same `CMakeLists.txt`
# where the .cpp files with the embind bindings are compiled.

# By convention, we require that every wasm module declared with `wasm_module()` have a test driver
# script in the tests directory of that component:

# 4. ./tests/test_{name}_wasm.js (e.g. test_hearts_wasm.js)

# Note that wasm modules may depend on code from other components. For example, the `hearts_wasm`
# artifacts depend on the `math_lib`, `cards_lib`, `tenzor_lib` components. The result is that `hearts_wasm`
# exposes all these wasm APIs. By convention, we generate separate wasm modules with their own unit tests,
# but there is no requirement that we generate wasm_modules for all components that a given
# wasm_module depends on.

function(wasm_module name)
if (EMSCRIPTEN)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(PARSE_ARGV 1 JS_WASM_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}")

    set(wasm_module "${name}_wasm")
    set(wasm_module_js "${wasm_module}.js")
    set(wasm_module_lib "${wasm_module}_lib")

    # There must be a js source in the **`tests/`** directory named test_${wasm_module}.js which is a test driver
    # for unit tests of the module.
    set(wasm_test_driver "test_${wasm_module}.js")

    add_executable(${wasm_module_js} ${PROJECT_SOURCE_DIR}/empty.cpp)

    add_library(${wasm_module_lib} INTERFACE)

    target_link_libraries(${wasm_module_lib}
        INTERFACE
        ${JS_WASM_TEST_DEPENDS}
    )

    target_link_libraries(${wasm_module_js} PUBLIC
        -Wl,--whole-archive
        ${wasm_module_lib} $<TARGET_PROPERTY:${wasm_module_lib},INTERFACE_LINK_LIBRARIES>
        -Wl,--no-whole-archive
    )

    target_link_options(
        ${wasm_module_js}
        PRIVATE --no-entry
        PRIVATE "SHELL:-s EXIT_RUNTIME=1"
        PRIVATE "SHELL:-s MODULARIZE=1"
        PRIVATE "SHELL:-s LLD_REPORT_UNDEFINED"
        PRIVATE "SHELL:-s EXPORT_NAME=${wasm_module}"
        PRIVATE "SHELL:-s EXPORT_ALL=1"
    )

    add_custom_target(install_${wasm_module}_test
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/tests/${wasm_test_driver} ${CMAKE_CURRENT_BINARY_DIR}/tests
        )

    add_custom_target(run_${wasm_module}_test
        COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${CMAKE_CURRENT_BINARY_DIR}/tests/${wasm_test_driver} ${PROJECT_BINARY_DIR}/bin/$<CONFIG>/${wasm_module}.js
    )

    add_dependencies(run_${wasm_module}_test
        ${wasm_module_js}
        install_${wasm_module}_test
    )

    add_dependencies(all_wasm_tests run_${wasm_module}_test)
endif()
endfunction()