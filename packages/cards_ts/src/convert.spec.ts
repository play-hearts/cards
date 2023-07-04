import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'

import factory from '@playhearts/gstate_wasm'
import type {GStateModule, Card} from '@playhearts/gstate_wasm'

chai.use(chaiAsPromised)
const {expect} = chai

const getMethods = (obj) => {
    let properties = new Set()
    let currentObj = obj
    do {
        Object.getOwnPropertyNames(currentObj).map((item) =>
            properties.add(item)
        )
    } while ((currentObj = Object.getPrototypeOf(currentObj)))
    return [...properties.keys()].filter(
        (item: string) => typeof obj[item] === 'function'
    )
}

describe('cards_ts', (): void => {
    let instance: GStateModule
    beforeEach(async () => {
        instance = await factory()
    })

    it('the Card member functions exist', (): void => {
        const card: Card = new instance.Card(0)
        expect(card).to.exist
        expect(card.ord).to.exist
        expect(card.suit).to.exist
        expect(card.rank).to.exist
        card.delete()
    })

    it('the Card member functions are typed as functions', (): void => {
        const card: Card = new instance.Card(0)
        expect(card).to.exist
        expect(card.ord).to.be.a('function')
        expect(card.suit).to.be.a('function')
        expect(card.rank).to.be.a('function')
        card.delete()
    })

    it('can create cards', (): void => {
        for (let ord = 0; ord < 52; ++ord) {
            const card: Card = new instance.Card(ord)
            expect(card.ord()).to.equal(ord)
            const r: number = card.rank().valueOf()
            const s: number = card.suit().valueOf()
            expect(s).to.equal(Math.floor(ord / 13))
            expect(r).to.equal(ord % 13)
            card.delete()
        }
    })
})
