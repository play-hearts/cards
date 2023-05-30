/// <reference types="@types/emscripten" />

import { Int126 } from "math_wasm";
import { Card, CardSet } from "cards_wasm";

declare module 'gstate_wasm' {

    export interface Deletable {
        delete: () => void;
    }

    export class GState implements Deletable {
        delete: () => null;
        passOffset: () => number;
        playerHands: (p: number) => CardSet;
        currentPlayersHand: () => CardSet;
        legalPlays: () => CardSet;
        setPassFor: (p: number, cards: CardSet) => void;
        startGame: () => void;
        playCard: (card: Card) => void;
        done: () => boolean;
        getPlayerOutcome: (p: number) => {zms: number, winPts: number};
    }

    export enum GameVariant {
        STANDARD = 0,
        JACK = 1,
        SPADES = 2,
    }

    type GStateConstructor = ((this: GState) => GState)
        | ((this: GState, v: {dealIndex: Int126, passOffset: number}, variant: GameVariant) => GState);

    export interface GStateModule extends EmscriptenModule {
        GState: GStateConstructor;

        getDealIndex: (gstate: GState) => Int126;

    }

    export interface GStateModuleFactory extends EmscriptenModuleFactory<GStateModule> {}
}
