// test_cards_jswasm.js

const [exe, script, wasmjs, ...args] = process.argv
const factory = require(wasmjs);
const assert = require('assert');

async function GState_test(instance) {
    return Promise.resolve()
    .then(() => {
        const gstate = new instance.GState();
        const dealIndex = instance.getDealIndex(gstate);
        gstate.delete()
    })
    .then(() => {
        const init = instance.GStateInit.kRandomVal();
        const gstate = new instance.GState(init, instance.GameVariant.STANDARD);
        const dealIndex = instance.getDealIndex(gstate);
        const passOffset = gstate.passOffset();
        gstate.delete();
        return {dealIndex, passOffset};
    })
    .then(({dealIndex, passOffset}) => {
        const init = instance.GStateInit.fromIndexAndOffset(dealIndex, passOffset);
        const gstate = new instance.GState(init, instance.GameVariant.STANDARD);
        const dealIndex2 = instance.getDealIndex(gstate);
        const passOffset2 = gstate.passOffset();
        assert.deepStrictEqual(dealIndex, dealIndex2);
        assert.equal(passOffset, passOffset2);
        gstate.delete();
    });
}

async function run() {
    const instance = await factory()
    console.log(`Succesfully loaded ${wasmjs}'`)

    await GState_test(instance);
}

run().then(() => console.log("Goodbye"))
