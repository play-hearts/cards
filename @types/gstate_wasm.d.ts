/// <reference types="@types/emscripten" />
/// <reference path="./math_wasm.d.ts" />
/// <reference path="./cards_wasm.d.ts" />

declare module "gstate_wasm" {

    import type { Int126 } from "math_wasm";
    import type { Card, CardSet, CardsModule } from "cards_wasm";

    export interface Deletable {
        delete: () => void;
    }

    export class GStateInit implements Deletable {
        delete: () => null;
    }

    export interface GState extends Deletable {
        delete: () => null;
        passOffset: () => number;
        playersHand: (p: number) => CardSet;
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

    export interface GStateModule extends CardsModule {
        GState: (this: GState, init: GStateInit, variant: GameVariant) => void;
        GStateInit: (this: GStateInit) => void;

        getDealIndex: (gstate: GState) => Int126;
        kRandomVal: () => GStateInit;
        fromIndexAndOffset: (index: Int126, offset: number) => GStateInit;
        fromVal: ({index: Int126, offset: number}) => GStateInit;

        GameVariant: typeof GameVariant;
    }

    export interface GStateModuleFactory extends EmscriptenModuleFactory<GStateModule> {}
}
