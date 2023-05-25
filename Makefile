# This Makefile exists largely just to document how to use cmake to configure, build, and run tests
# for the two main platforms (x86 & emcc).

X86=builds/x86/
EMCC=builds/emcc/
GENERATOR="Ninja Multi-Config"
CMAKE=/home/jim/.local/bin/cmake

.PHONY: all configure_x86 configure_emcc build_x86 build_emcc test_x86 test_emccgit

all: test

${X86}:
	mkdir -p ${X86}

${EMCC}:
	mkdir -p ${EMCC}

configure_x86: ${X86}
	${CMAKE} -S . -B ${X86} -G ${GENERATOR}

configure_emcc : ${EMCC}
	emcmake ${CMAKE} -S . -B ${EMCC} -G ${GENERATOR}

build_x86: configure_x86
	${CMAKE} --build ${X86}

build_emcc: configure_emcc
	${CMAKE} --build ${EMCC}

test_x86: build_x86
	${CMAKE} --build ${X86} -t test

test_emcc: build_emcc
	${CMAKE} --build ${EMCC} -t test

build: build_x86 build_emcc

wasm_tests: test_emcc
	${CMAKE} --build ${EMCC} -t all_wasm_tests

test: test_x86 test_emcc wasm_tests
