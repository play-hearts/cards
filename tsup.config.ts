import { defineConfig } from 'tsup'

export default defineConfig({
  entry: [
    "./src/api.mts",
    "./builds/emcc/bin/Debug/gstate_wasm.js",
    "./builds/emcc/bin/Debug/gstate_wasm.wasm"
  ],
  format: [
    "esm"
  ],
splitting: false,
  sourcemap: true,
  clean: true,
  loader: {
    ".wasm": "copy"
  }
})
