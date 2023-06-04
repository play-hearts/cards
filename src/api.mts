/// <reference types="@types/emscripten" />
/// <reference path="../@types/gstate_wasm.d.ts" />

// app.mts
import factory from "../builds/emcc/bin/Debug/gstate_wasm.js";
import { GStateModule, GStateModuleFactory } from "gstate_wasm";
import { Card, CardSet, GState } from "gstate_wasm";

export const instanceP: Promise<GStateModule> = (factory as GStateModuleFactory)();

export async function playOutGame(instance: GStateModule, gstate: GState): Promise<void> {

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
