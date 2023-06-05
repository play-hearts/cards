# gstate_wasm
### A WebAssembly implementation of a cards game state implemented in C++.

This is a work in progress.

This package provides access to a C++ implementation of the card game Hearts
and other related trick-based card games such as Spades.

The pure C++ library is highly optimized for running large numbers of Monte
Carlo game simulations to generate training data for a Machine Learning
implementation of algoritmmic player "agents".

This NodeJS implementation exists to facilitate a web-based implementation
of these card games.

## Using this package

See the test code in packages/gstate_wasm_test/src/api.spec.ts, which
demonstrates how to use the API. The Typescript iterface is provided in the
file `gstate_wasm.d.ts`.
