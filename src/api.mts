/// <reference types="@types/emscripten" />
/// <reference path="../@types/math_wasm.d.ts" />
/// <reference path="../@types/cards_wasm.d.ts" />
/// <reference path="../@types/gstate_wasm.d.ts" />

// app.mts
import factory from "@wasm/gstate_wasm.js";
import assert from 'node:assert';

import type { Int126 } from 'math_wasm';
import type { Card, CardSet } from 'cards_wasm';
import type { GStateModule, GStateModuleFactory, GState } from 'gstate_wasm';

export const instanceP = (factory as GStateModuleFactory)();

async function withInitTest(instance:GStateModule): Promise<{ dealIndex: Int126, passOffset: number }> {
    const init = instance.kRandomVal();
    const gstate: GState = new instance.GState(init, instance.GameVariant.STANDARD);
    const dealIndex: Int126 = instance.getDealIndex(gstate);
    const passOffset: number = gstate.passOffset();
    console.log("dealIndex, passOffset:", dealIndex, passOffset);
    gstate.delete();
    return { dealIndex, passOffset };
}

async function playOutGame(instance: GStateModule, gstate: GState): Promise<void> {

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
        const legal: CardSet = gstate.legalPlays();
        const card: Card = instance.aCardAtRandom(legal);
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

async function playOutGameTest(instance: GStateModule, dealIndex: Int126, passOffset: number): Promise<void> {
    const init = instance.fromIndexAndOffset(dealIndex, passOffset);
    const gstate = new instance.GState(init, instance.GameVariant.STANDARD);
    const dealIndex2 = instance.getDealIndex(gstate);
    const passOffset2 = gstate.passOffset();
    assert.deepStrictEqual(dealIndex, dealIndex2);
    assert.equal(passOffset, passOffset2);
    await playOutGame(instance, gstate);
    assert.equal(gstate.done(), true);
    gstate.delete();
}


instanceP
.then(async (instance: GStateModule): Promise<void> => {
    const { dealIndex, passOffset} = await withInitTest(instance);
    await playOutGameTest(instance, dealIndex, passOffset);
})
