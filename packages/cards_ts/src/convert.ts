

import type { CardType, Suit, Rank, SuitOrd, RankOrd } from './cards.js';
import type { Card, CardSet, CardVector, GStateModule } from '@playhearts/gstate_wasm';
import { suitName, rankName, suitOrd, rankOrd } from './cards.js';

export function convertCard(card: Card): CardType {
    const suit: Suit = suitName(card.suit() as number);
    const rank: Rank = rankName(card.rank() as number);
    card.delete();
    return { suit, rank };
}

export function convertCardSet(cardSet: CardSet): CardType[] {
    const cardVector: CardVector = cardSet.asCardVector();
    cardSet.delete();
    let converted: CardType[] = [];
    for (let i = 0; i < cardVector.size(); i++) {
        converted.push(convertCard(cardVector.get(i)));
    }
    return converted;
}

export function makeCard(instance: GStateModule, suit: Suit, rank: Rank): Card {
    const s: SuitOrd = suitOrd(suit);
    const r: RankOrd = rankOrd(rank);
    const c: number = s*13 + r;
    return new instance.Card(c);
}
