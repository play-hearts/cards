/// <reference types="emscripten" />

export interface Deletable {
    delete: () => void;
}

export interface RandomGenerator extends Deletable {
    delete: () => null;
    randNorm: () => number;
}

export enum Suit {
    kClubs = 0,
    kDiamonds = 1,
    kSpades = 2,
    kHearts = 3,
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

export type TrickRep = [Card, Card, Card, Card];
export type TrickOrdRep = [number, number, number, number];

export interface Trick extends Deletable {
    delete: () => null;
    at: (i: number) => Card;
    trickSuit: () => Suit;
    lead: () => number;
    leadCard: () => Card;
    winner: () => number;
    highCard: () => Card;
    getTrickPlay: () => Card;
    rep: () => TrickRep;
    ordRep: () => TrickOrdRep;
}

// GStateInit is a emscripten::value_object;
export interface GStateInit {
    dealHexStr: string,
    passOffset: number,
}

export interface GState extends Deletable {
    delete: () => null;

    currentPlayer: () => number;
    currentPlayersHand: () => CardSet;
    currentTrick: () => Trick;
    done: () => boolean;
    getPlayerOutcome: (p: number) => { zms: number, winPts: number };
    legalPlays: () => CardSet;
    passOffset: () => number;
    playCard: (card: Card) => void;
    playersHand: (p: number) => CardSet;
    playIndex: () => number;
    priorTrick: () => Trick;
    setPassFor: (p: number, cards: CardSet) => void;
    startGame: () => void;
}

export enum GameVariant {
    STANDARD = 0,
    JACK = 1,
    SPADES = 2,
}

export interface GStateModule extends EmscriptenModule {
    RandomGenerator: new () => RandomGenerator;
    Deal: new (dealIndex: string) => Deal;
    CardSet: new () => CardSet;
    Card: new (ord: number) => Card;
    CardVector: new () => CardVector;
    suitOf: (card: Card) => Suit;
    rankOf: (card: Card) => Rank;
    cardFor: (suit: Suit, rank: Rank) => Card;
    nameOfCard: (card: Card) => string;
    nameOfSuit: (suit: Suit) => string;
    to_string: (cardSet: CardSet) => string;
    aCardAtRandom: (cardSet: CardSet) => Card;
    chooseThreeAtRandom: (cardSet: CardSet) => CardSet;
    GState: new (init: GStateInit, variant: GameVariant) => GState;

    getDealIndex: (gstate: GState) => string;
    kRandomVal: () => GStateInit;

    GameVariant: typeof GameVariant;

    Suit: typeof Suit;
    Rank: typeof Rank;
}

export interface GStateModuleFactory extends EmscriptenModuleFactory<GStateModule> { }

declare const Module: GStateModuleFactory;
export default Module;
