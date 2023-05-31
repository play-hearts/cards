import { defineConfig } from 'tsup'

export default defineConfig({
  entry: [
    "./src/api.mts",
    "./builds/emcc/bin/Debug/cards_wasm.js",
    "./builds/emcc/bin/Debug/gstate_wasm.js",
    "./builds/emcc/bin/Debug/math_wasm.js",
    "./@types/math_wasm.d.ts",
  ],
  format: [
    "esm"
  ],
  dts: "./src/api.mts",
  splitting: false,
  sourcemap: true,
  clean: true,
  target: "node18"
})
