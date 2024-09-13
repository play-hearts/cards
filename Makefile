# This Makefile exists largely just to document how to use cmake to configure, build, and run tests
# for the two main platforms (GCC & emcc).

GCC=builds/gcc/
EMCC=builds/emcc/
CLANG=builds/clang/
GENERATOR="Ninja Multi-Config"
CMAKE ?= $(shell which cmake) # /home/jim/.local/bin/cmake
NODE ?= $(shell which node) # /usr/local/bin/node
CONFIG ?= Debug

.PHONY: all configure_GCC configure_emcc build_GCC build_emcc test_GCC test_emcc build wasm_tests tsbuild dist dist_test clean api_test test

all: test

${GCC}:
	mkdir -p ${GCC}

# ${EMCC}:
# 	mkdir -p ${EMCC}

${GCC}/CMakeCache.txt: ${GCC}
	${CMAKE} -S . -B ${GCC} -G ${GENERATOR} --toolchain=cmake/posix.toolchain.cmake

configure_GCC: ${GCC}/CMakeCache.txt

# ${EMCC}/CMakeCache.txt : ${EMCC}
# 	emcmake ${CMAKE} -S . -B ${EMCC} -G ${GENERATOR} -D CMAKE_CROSSCOMPILING_EMULATOR=${NODE}

# configure_emcc : ${EMCC}/CMakeCache.txt

reconfigure_GCC :
	rm -rf  ${GCC}/CMakeCache.txt
	${MAKE} configure_GCC

# reconfigure_emcc :
# 	rm -rf  ${EMCC}/CMakeCache.txt
# 	${MAKE} configure_emcc

build_GCC: configure_GCC
	${CMAKE} --build ${GCC}  --config ${CONFIG}

# build_emcc: configure_emcc
# 	${CMAKE} --build ${EMCC}  --config ${CONFIG}
# 	cp ${EMCC}bin/${CONFIG}/gstate_wasm.* packages/gstate_wasm/

build_js: build_emcc
	pnpm -r build

test_GCC: build_GCC
	${CMAKE} --build ${GCC} -t test  --config ${CONFIG}

# test_emcc: build_emcc
# 	${CMAKE} --build ${EMCC} -t test  --config ${CONFIG}

# test_js: build_js
# 	pnpm -r test

build: build_GCC build_clang # build_emcc build_js

clean:
	rm -rf builds packages/gstate_wasm_test/dist packages/gstate_wasm/gstate_wasm.*.js

test: test_GCC test_clang # test_emcc test_js


${CLANG}:
	mkdir -p ${CLANG}

configure_clang: ${CLANG}
	${CMAKE} -S . -B ${CLANG} -G ${GENERATOR} -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ${EXTRA_OPTS}  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}

build_clang: configure_clang
	${CMAKE} --build ${CLANG} --config ${CONFIG}

test_clang: build_clang
	${CMAKE} --build ${CLANG} --config ${CONFIG} -t test

clang: test_clang
