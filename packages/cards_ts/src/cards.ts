// Path: packages/cards_ts/src/cards.ts

// The strings in all_suits and all_ranks are used in the SVG image filenames.
// The UI shouldn't need to even know the ordering of either the suits or the ranks,
// i.e. it should need to know that ace > king > queen ...;
// nor should it need to know that hearts > spades > diamonds > clubs.
// However game logic does depend on this ordering, so there will be some need for translation, TBD.

export const all_suits = ['clubs', 'diamonds', 'spades', 'hearts'] as const;
export type Suit = typeof all_suits[number];
export type SuitOrd = typeof all_suits.indexOf;

export const all_ranks = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'jack', 'queen', 'king', 'ace'] as const;
export type Rank = typeof all_ranks[number];
export type RankOrd = typeof all_ranks.indexOf;

export type CardType = {
    suit: Suit;
    rank: Rank;
};

export const kNoCard = { suit: "", rank: "" } as const;
export type NoCard = typeof kNoCard;

export type TrickCard = {
    suit: Suit | "";
    rank: Rank | "";
};

export type TrickType = [TrickCard, TrickCard, TrickCard, TrickCard];
export const kEmptyTrick = [kNoCard, kNoCard, kNoCard, kNoCard] as const;

export type CardHand = CardType[];
export type FourHands = [CardHand, CardHand, CardHand, CardHand];

export const all_seat_names = ['south', 'west', 'north', 'east'] as const;
export type SeatName = typeof all_seat_names[number];

export function seatPosition(seat: SeatName): number {
    return all_seat_names.indexOf(seat);
}

export function seatName(seat: number): SeatName {
    return all_seat_names[seat];
}

export const kSouthNumber = seatPosition('south');
export const kWestNumber = seatPosition('west');
export const kNorthNumber = seatPosition('north');
export const kEastNumber = seatPosition('east');

export function isSuit(suit: unknown): suit is Suit {
    return all_suits.includes(suit as Suit);
}

export function isRank(rank: unknown): rank is Rank {
    return all_ranks.includes(rank as Rank);
}

export function suitOrd(suit: Suit): number {
    if (!isSuit(suit))
        throw new Error(`Invalid SuitOrd: ${suit}`);
    return all_suits.indexOf(suit);
}

export function rankOrd(rank: Rank): number {
    if (!isRank(rank))
        throw new Error(`Invalid Rank: ${rank}`);
    return all_ranks.indexOf(rank);
}

export function rankName(rankOrd: number): Rank {
    if (!isRankOrd(rankOrd))
        throw new Error(`Invalid rankOrd: ${rankOrd}`);
    return all_ranks[rankOrd];
}

export function suitName(suitOrd: number): Suit {
    if (!isSuitOrd(suitOrd))
        throw new Error(`Invalid suit: ${suitOrd}`);
    return all_suits[suitOrd];
}

export function isSuitOrd(suitOrd: unknown): suitOrd is SuitOrd {
    return typeof suitOrd === 'number' && suitOrd >= 0 && suitOrd < 4;
}

export function isRankOrd(rankOrd: unknown): rankOrd is RankOrd {
    return typeof rankOrd === 'number' && rankOrd >= 0 && rankOrd < 13;
}

export function isCard(card: unknown): card is CardType {
    return isSuit((card as CardType).suit) && isRank((card as CardType).rank);
}

export function isNoCard(card: unknown): card is NoCard {
    return (card as NoCard).suit === "" && (card as NoCard).rank === "";
}

export function isTrickCard(card: unknown): card is TrickCard {
    return ((card as TrickCard).suit === "" && (card as TrickCard).rank === "")
        || ((isSuit((card as TrickCard).suit) && isRank((card as TrickCard).rank)));
}

export function isTrick(trick: unknown): trick is TrickType {
    return Array.isArray(trick) && trick.length === 4 && trick.every(isTrickCard);
}
