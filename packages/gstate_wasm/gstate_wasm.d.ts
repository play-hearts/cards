/// <reference types="emscripten" />

export interface Deletable {
    delete: () => void;
}

export interface Int126 {
    hi: BigInt;
    lo: BigInt;
}

export interface RandomGenerator extends Deletable {
    delete: () => null;
    randNorm: () => number;
}

export enum Suit {
    kClubs = 0,
    kDiamonds = 1,
    kHearts = 2,
    kSpades = 3,
}

export enum Rank {
    kTwo = 0,
    kThree = 1,
    kFour = 2,
    kFive = 3,
    kSix = 4,
    kSeven = 5,
    kEight = 6,
    kNine = 7,
    kTen = 8,
    kJack = 9,
    kQueen = 10,
    kKing = 11,
    kAce = 12,
}

export interface Card extends Deletable {
    ord: () => number;
    suit: () => Suit;
    rank: () => Rank;
}

export interface CardVector extends Deletable {
    size: () => number;
    get: (i: number) => Card;
}

export interface CardSet extends Deletable {
    delete: () => null;
    asCardVector: () => CardVector;
    size: () => number;
    setUnion: (other: CardSet) => CardSet;
    setIntersection: (other: CardSet) => CardSet;
    setSubtract: (other: CardSet) => CardSet;
    equal: (other: CardSet) => boolean;
}

export interface Deal extends Deletable {
    dealFor: (p: number) => CardSet;
    indexAsHexString: () => string;
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
    trickSuit: () => Suit;
}

export enum GameVariant {
    STANDARD = 0,
    JACK = 1,
    SPADES = 2,
}

export interface GStateModule extends EmscriptenModule {
    RandomGenerator: (this: RandomGenerator) => void;
    Deal: (this: Deal, dealIndex: string) => void;
    CardSet: (this: CardSet) => void;
    Card: (this: Card, ord: number) => void;
    CardVector: (this: CardVector) => void;
    suitOf: (card: Card) => Suit;
    rankOf: (card: Card) => Rank;
    cardFor: (suit: Suit, rank: Rank) => Card;
    nameOfCard: (card: Card) => string;
    nameOfSuit: (suit: Suit) => string;
    to_string: (cardSet: CardSet) => string;
    aCardAtRandom: (cardSet: CardSet) => Card;
    chooseThreeAtRandom: (cardSet: CardSet) => CardSet;
    GState: (this: GState, init: GStateInit, variant: GameVariant) => void;
    GStateInit: (this: GStateInit) => void;

    getDealIndex: (gstate: GState) => Int126;
    kRandomVal: () => GStateInit;
    fromIndexAndOffset: (index: Int126, offset: number) => GStateInit;
    fromVal: ({index: Int126, offset: number}) => GStateInit;

    GameVariant: typeof GameVariant;
    Suit: typeof Suit;
    Rank: typeof Rank;
}

export interface GStateModuleFactory extends EmscriptenModuleFactory<GStateModule> {}

declare const Module: GStateModuleFactory;
export default Module;
