import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'

import factory, { Trick, TrickOrdRep } from '@playhearts/gstate_wasm'

import type * as gstate_wasm from '@playhearts/gstate_wasm'

chai.use(chaiAsPromised)
const { expect } = chai

export async function playOutGame(instance: gstate_wasm.GStateModule, gstate: gstate_wasm.GState): Promise<void> {
    const passOffset = gstate.passOffset()
    if (passOffset > 0) {
        for (let i = 0; i < 4; i++) {
            let hand = gstate.playersHand(i)
            let pass = instance.chooseThreeAtRandom(hand)
            gstate.setPassFor(i, pass)
        }
    }
    gstate.startGame()

    while (!gstate.done()) {
        const legal: gstate_wasm.CardSet = gstate.legalPlays()
        const card: gstate_wasm.Card = instance.aCardAtRandom(legal)
        const p: number = gstate.currentPlayer()
        expect(p).to.be.within(0, 3)
        const taken: gstate_wasm.CardSet = gstate.takenBy(
            (p + gstate.trickLead()) % 4
        )
        taken.delete()

        gstate.playCard(card)
        card.delete()
        legal.delete()
        const trick: Trick = gstate.currentTrick()
        expect(trick).to.not.be.undefined
        const trickArr: TrickOrdRep = trick.ordRep()
        for (let i = 0; i < 4; ++i) {
            const card: gstate_wasm.Card = trick.at(i)
            const x = (4 + i - gstate.trickLead()) % 4
            if (x < gstate.playInTrick()) {
                expect(card.ord()).to.equal(gstate.getTrickPlay(x).ord())
            }
            expect(card.ord()).to.equal(trickArr[i])
            card.delete()
        }
        // console.log('typeof trickArr: ', typeof trickArr, Array.isArray(trickArr));
        // console.log('trickArr: ', trickArr);
        expect(trickArr.length).to.equal(4)
        trick.delete()
        const prior: Trick = gstate.priorTrick()
        expect(prior).to.not.be.undefined
        prior.delete()
    }

    const playerScores: gstate_wasm.PlayerScores = gstate.getPlayerScores()

    for (let p = 0; p < 4; ++p) {
        const outcome = gstate.getPlayerOutcome(p)
        console.log(
            `Player ${p} zms: ${outcome.zms}, winPts: ${outcome.winPts}`
        )
        expect(outcome.zms).to.equal(playerScores[p])
    }
}

describe('api', (): void => {
    let instance: gstate_wasm.GStateModule
    beforeEach(async () => { instance = await factory() })

    describe('math api', (): void => {
        it('can create a RandomGenerator', async () => {
            const rng: gstate_wasm.RandomGenerator =
                new instance.RandomGenerator()
            expect(rng).not.to.be.undefined
            rng.delete()
        })
    })

    describe('cards api', (): void => {
        it('can create a CardSet', async () => {
            const cards: gstate_wasm.CardSet = new instance.CardSet()
            expect(cards.size()).to.equal(0)
        })

        it('can create a random Deal and recreate it', async () => {
            const deal1: gstate_wasm.Deal = new instance.Deal('')
            const index: string = deal1.indexAsHexString()
            expect(index).to.have.lengthOf(25)
            const deal2: gstate_wasm.Deal = new instance.Deal(index)
            for (let p = 0; p < 4; ++p) {
                const hand1 = deal1.dealFor(p)
                const hand2 = deal2.dealFor(p)
                expect(hand1.equal(hand2)).to.be.true
                hand1.delete()
                hand2.delete()
            }
            deal1.delete()
            deal2.delete()
        })

        it('a deal is consistent: four non-overlapping subsets', async () => {
            const deal: gstate_wasm.Deal = new instance.Deal('')
            let check: gstate_wasm.CardSet = new instance.CardSet()
            expect(check.size()).to.equal(0)
            for (let p = 0; p < 4; ++p) {
                const hand: gstate_wasm.CardSet = deal.dealFor(p)
                expect(hand.size()).to.equal(13)
                const inCommon: gstate_wasm.CardSet =
                    check.setIntersection(hand)
                expect(inCommon.size()).to.equal(0)
                check = check.setUnion(hand)
                hand.delete()
                inCommon.delete()
            }
            expect(check.size()).to.equal(52)
            check.delete()
            deal.delete()
        })
    })

    describe('gstate api', (): void => {
        it('withInitTest', async (): Promise<void> => {
            const init: gstate_wasm.GStateInit = instance.kRandomVal()
            console.log('kRandomVal:', init)
            const gstate: gstate_wasm.GState = new instance.GState(
                init,
                instance.GameVariant.STANDARD
            )
            const dealHexStr: string = instance.getDealIndex(gstate)
            expect(dealHexStr).to.equal(init.dealHexStr)
            const passOffset: number = gstate.passOffset()
            expect(passOffset).to.equal(init.passOffset)
            gstate.delete()

            const init2: gstate_wasm.GStateInit = { dealHexStr, passOffset }
            const gstate2: gstate_wasm.GState = new instance.GState(
                init2,
                instance.GameVariant.STANDARD
            )
            const dealIndex2: string = instance.getDealIndex(gstate2)
            const passOffset2: number = gstate2.passOffset()
            expect(dealIndex2).to.deep.equal(dealHexStr)
            expect(passOffset2).to.equal(passOffset)
            gstate2.delete()
        })

        it('playOutGameTest', async (): Promise<void> => {
            const init: gstate_wasm.GStateInit = instance.kRandomVal()
            const gstate: gstate_wasm.GState = new instance.GState(
                init,
                instance.GameVariant.STANDARD
            )
            await playOutGame(instance, gstate)
            gstate.delete()
        })
    })
})
