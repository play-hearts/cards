// test_gstate_wasm.mjs

import assert from 'node:assert';
import factory from './gstate_wasm.js';

async function playOutGame(instance, gstate) {

    const passOffset = gstate.passOffset();
    if (passOffset > 0) {
        for (let i = 0; i < 4; i++) {
            let hand = gstate.playersHand(i);
            let pass = instance.chooseThreeAtRandom(hand);
            gstate.setPassFor(i, pass);
        }
    }
    gstate.startGame();

    while (!gstate.done()) {
        const legal = gstate.legalPlays();
        const card = instance.aCardAtRandom(legal);
        gstate.playCard(card);
        card.delete();
        legal.delete();
    }

    for (let p=0; p<4; ++p)
    {
        const outcome = gstate.getPlayerOutcome(p);
        console.log(`Player ${p} zms: ${outcome.zms}, winPts: ${outcome.winPts}`);
    }
}

async function GState_test(instance) {
    return Promise.resolve()
    .then(() => {
        const init = instance.kRandomVal();
        const gstate = new instance.GState(init, instance.GameVariant.STANDARD);
        const dealIndex = instance.getDealIndex(gstate);
        const passOffset = gstate.passOffset();
        gstate.delete();
        return {dealIndex, passOffset};
    })
    .then(async ({dealIndex, passOffset}) => {
        const init = instance.fromIndexAndOffset(dealIndex, passOffset);
        const gstate = new instance.GState(init, instance.GameVariant.STANDARD);
        const dealIndex2 = instance.getDealIndex(gstate);
        const passOffset2 = gstate.passOffset();
        assert.deepStrictEqual(dealIndex, dealIndex2);
        assert.equal(passOffset, passOffset2);
        await playOutGame(instance, gstate);
        assert.equal(gstate.done(), true);
        gstate.delete();
    });
}

async function run() {
    const instance = await factory()

    await GState_test(instance);
}

run().then(() => console.log("Goodbye"))
