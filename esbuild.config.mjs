import * as esbuild from 'esbuild'

await esbuild.build({
    entryPoints: [
        'src/api.mts',
        'builds/emcc/bin/Debug/gstate_wasm.js',
        '@types/gstate_wasm.d.ts',
    ],
    loader: { '.d.ts': 'copy' },
    bundle: true,
    outdir: 'dist',
    format: 'esm',
    target: 'es2020',
    platform: 'node',
    sourcemap: true,
})
