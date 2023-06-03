import { defineConfig } from 'tsup'

export default defineConfig({
  entry: [
    "./src/api.mts",
    "./builds/emcc/bin/Debug/gstate_wasm.js",
  ],
  format: [
    "esm"
  ],
  banner: {
    js: `import {createRequire as __createRequire} from 'module';var require=__createRequire(import\.meta.url);`,
  },
  dts: "./src/api.ts",
  splitting: false,
  sourcemap: true,
  clean: true,
  target: "node18"
})
