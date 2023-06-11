# This Makefile exists largely just to document how to use cmake to configure, build, and run tests
# for the two main platforms (x86 & emcc).

X86=builds/x86/
EMCC=builds/emcc/
GENERATOR="Ninja Multi-Config"
CMAKE ?= $(shell which cmake) # /home/jim/.local/bin/cmake
NODE ?= $(shell which node) # /usr/local/bin/node
CONFIG ?= Debug

.PHONY: all configure_x86 configure_emcc build_x86 build_emcc test_x86 test_emcc build wasm_tests tsbuild dist dist_test clean api_test test

all: test

${X86}:
	mkdir -p ${X86}

${EMCC}:
	mkdir -p ${EMCC}

${X86}/CMakeCache.txt: ${X86}
	${CMAKE} -S . -B ${X86} -G ${GENERATOR}

configure_x86: ${X86}/CMakeCache.txt

${EMCC}/CMakeCache.txt : ${EMCC}
	emcmake ${CMAKE} -S . -B ${EMCC} -G ${GENERATOR} -D CMAKE_CROSSCOMPILING_EMULATOR=${NODE}

configure_emcc : ${EMCC}/CMakeCache.txt

reconfigure_x86 :
	rm -rf  ${X86}/CMakeCache.txt
	${MAKE} configure_x86

reconfigure_emcc :
	rm -rf  ${EMCC}/CMakeCache.txt
	${MAKE} configure_emcc

build_x86: configure_x86
	${CMAKE} --build ${X86}  --config ${CONFIG}

build_emcc: configure_emcc
	${CMAKE} --build ${EMCC}  --config ${CONFIG}
	cp ${EMCC}bin/${CONFIG}/gstate_wasm.* packages/gstate_wasm/

build_js:
	pnpm -r build

test_x86: build_x86
	${CMAKE} --build ${X86} -t test  --config ${CONFIG}

test_emcc: build_emcc
	${CMAKE} --build ${EMCC} -t test  --config ${CONFIG}

test_js: build_js
	pnpm -r test

build: build_x86 build_emcc build_js

clean:
	rm -rf builds packages/gstate_wasm_test/dist packages/gstate_wasm/gstate_wasm.*.js

test: test_x86 test_emcc test_js
