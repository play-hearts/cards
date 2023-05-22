// test_math_jswasm.js

const [exe, script, wasmjs, ...args] = process.argv
const factory = require(wasmjs);
const assert = require('assert');

// async function math_val_test(instance) {

//     const a = {hi: 0x12345678n, lo: 0x9abcdef0n};
//     const b = {hi: 0x0n, lo: 0x1n};
//     const c = instance.add_uint128(a, b);
//     const d = instance.add_uint128(b, a);

//     assert.deepStrictEqual(c, d);

// }


async function Int128_test(instance) {


}


async function run() {
    const instance = await factory()
    console.log(`Succesfully loaded ${wasmjs}'`)

    // await math_val_test(instance);
    // await math_test(instance);
}

run().then(() => console.log("Goodbye"))
