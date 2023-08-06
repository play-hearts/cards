import type { CardType, SuitName, RankName } from './cards.js'
import type { Card, CardSet, CardVector, GStateModule } from '@playhearts/gstate_wasm'
import { suitName, rankName } from './cards.js'

export function convertCard(card: Card): CardType {
    const suit: SuitName = suitName(card.suit() as number)
    const rank: RankName = rankName(card.rank() as number)
    card.delete()
    return { suit, rank }
}

export function cardToOrd(card: Card): number {
    const ord: number = card.ord()
    card.delete()
    return ord
}

export function convertCardSet(cardSet: CardSet): CardType[] {
    const cardVector: CardVector = cardSet.asCardVector()
    cardSet.delete()
    let converted: CardType[] = []
    for (let i = 0; i < cardVector.size(); i++) {
        converted.push(convertCard(cardVector.get(i)))
    }
    return converted
}

export function cardSetAsOrds(cardSet: CardSet): number[] {
    const cardVector: CardVector = cardSet.asCardVector()
    cardSet.delete()
    let converted: number[] = []
    for (let i = 0; i < cardVector.size(); i++) {
        converted.push(cardToOrd(cardVector.get(i)))
    }
    return converted
}

export function makeCard(instance: GStateModule, s: number, r: number): Card {
    const c: number = s * 13 + r
    return new instance.Card(c)
}
