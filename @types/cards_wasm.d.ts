/// <reference types="@types/emscripten" />

declare module 'cards_wasm' {
    export interface Deletable {
        delete: () => void;
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

    export interface CardsModule extends EmscriptenModule {
        Deal: (this: Deal, dealIndex: string) => void;
        CardSet: (this: CardSet) => void;
        Card: (this: Card) => void;
        CardVector: (this: CardVector) => void;
        suitOf: (card: Card) => Suit;
        rankOf: (card: Card) => Rank;
        cardFor: (suit: Suit, rank: Rank) => Card;
        nameOfCard: (card: Card) => string;
        nameOfSuit: (suit: Suit) => string;
        to_string: (cardSet: CardSet) => string;
        aCardAtRandom: (cardSet: CardSet) => Card;
        chooseThreeAtRandom: (cardSet: CardSet) => CardSet;
    }

    export interface CardsModuleFactory extends EmscriptenModuleFactory<CardsModule> {}
}
