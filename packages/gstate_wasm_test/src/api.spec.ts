
import chai from 'chai';
import chaiAsPromised from 'chai-as-promised';

import factory from "@playhearts/gstate_wasm";

import type { Int126, RandomGenerator, Card, CardSet, Deal, GStateInit, GStateModule, GState } from '@playhearts/gstate_wasm';

chai.use(chaiAsPromised);
const { expect } = chai;

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

describe('api', (): void => {
    let instance: GStateModule;
    beforeEach(async () => {
        instance = await factory();
    });

    describe('math api', (): void => {
        it('can create a RandomGenerator', async() => {
            const rng: RandomGenerator = new instance.RandomGenerator();
            expect(rng).not.to.be.undefined;
            rng.delete();
        });
    });

    describe('cards api', (): void => {
        it('can create a CardSet', async() => {
            const cards: CardSet = new instance.CardSet();
            expect(cards.size()).to.equal(0);
        });

        it('can create a random Deal and recreate it', async() => {
            const deal1: Deal = new instance.Deal("");
            const index: string = deal1.indexAsHexString();
            expect(index).to.have.lengthOf(25);
            const deal2: Deal = new instance.Deal(index);
            for (let p=0; p<4; ++p) {
                const hand1 = deal1.dealFor(p);
                const hand2 = deal2.dealFor(p);
                expect(hand1.equal(hand2)).to.be.true;
                hand1.delete();
                hand2.delete();
            }
            deal1.delete();
            deal2.delete();
        });

        it('a deal is consistent: four non-overlapping subsets', async() => {
            const deal: Deal = new instance.Deal("");
            let check: CardSet = new instance.CardSet();
            expect(check.size()).to.equal(0);
            for (let p=0; p<4; ++p) {
                const hand: CardSet = deal.dealFor(p);
                expect(hand.size()).to.equal(13);
                const inCommon: CardSet = check.setIntersection(hand);
                expect(inCommon.size()).to.equal(0);
                check = check.setUnion(hand);
                hand.delete();
                inCommon.delete();
            }
            expect(check.size()).to.equal(52);
            check.delete();
            deal.delete();
        });
    });

    describe('gstate api', (): void => {
        it('withInitTest', async (): Promise<void> => {
            const init: GStateInit = instance.kRandomVal();
            const gstate: GState = new instance.GState(init, instance.GameVariant.STANDARD);
            const dealIndex: Int126 = instance.getDealIndex(gstate);
            const passOffset: number = gstate.passOffset();
            console.log("dealIndex, passOffset:", dealIndex, passOffset);
            gstate.delete();
            expect(dealIndex).not.to.be.undefined;
            expect(passOffset).not.to.be.undefined;

            const init2: GStateInit = instance.fromIndexAndOffset(dealIndex, passOffset);
            const gstate2: GState = new instance.GState(init2, instance.GameVariant.STANDARD);
            const dealIndex2: Int126 = instance.getDealIndex(gstate2);
            const passOffset2: number = gstate2.passOffset();
            console.log("dealIndex2, passOffset2:", dealIndex2, passOffset2);
            expect(dealIndex2).to.deep.equal(dealIndex);
            expect(passOffset2).to.equal(passOffset);
            gstate2.delete();
        });

        it('playOutGameTest', async (): Promise<void> => {
            const init: GStateInit = instance.kRandomVal();
            const gstate: GState = new instance.GState(init, instance.GameVariant.STANDARD);
            await playOutGame(instance, gstate);
            gstate.delete();
        });
    });
});
