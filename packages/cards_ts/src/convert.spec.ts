import chai from 'chai';
import chaiAsPromised from 'chai-as-promised';

import factory from "@playhearts/gstate_wasm";
import type { GStateModule, Card, SuitEnum, RankEnum } from '@playhearts/gstate_wasm';

chai.use(chaiAsPromised);
const { expect } = chai;

describe('cards_ts', (): void => {

    let instance: GStateModule;
    beforeEach(async () => {
        instance = await factory();
    });

    it('can create cards', (): void => {
        for (let ord=0; ord<52; ++ord)
        {
            const card: Card = new instance.Card(ord);
            const suit: number = card.suit().valueOf();
            const rank: number = card.rank().valueOf();
            expect(suit).to.equal(Math.floor(ord/13));
            expect(rank).to.equal(ord%13);
            card.delete();
        }
    });

});
