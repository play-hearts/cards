// test_cards_jswasm.js

const [exe, script, wasmjs, ...args] = process.argv
const factory = require(wasmjs);
const assert = require('assert');


async function GState_test(instance) {
    const gstate = new instance.GState();

    gstate.delete()
}

async function run() {
    const instance = await factory()
    console.log(`Succesfully loaded ${wasmjs}'`)

    await GState_test(instance);
}

run().then(() => console.log("Goodbye"))
