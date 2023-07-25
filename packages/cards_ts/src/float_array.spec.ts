import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'

import factory from '@playhearts/gstate_wasm'
import type { GStateModule, GStateInit, GState } from '@playhearts/gstate_wasm'
import { newFloatArray, freeFloatArray, fillProbabilities, asMin2022InputTensor, type FloatArraySpec } from './float_array.js'

chai.use(chaiAsPromised)
const { expect } = chai

describe('floatarray', (): void => {
    let instance: GStateModule
    beforeEach(async () => {
        instance = await factory()
    })

    it('_malloc exists ', (): void => {
        expect(instance._malloc).to.exist
        expect(instance._malloc).to.be.a("function")
    })

    it('_free exists ', (): void => {
        expect(instance._free).to.exist
        expect(instance._free).to.be.a("function")
    })

    it('can make and free a FloatArraySpec ', (): void => {
        const spec: FloatArraySpec = newFloatArray(instance, 3)
        expect(spec).to.exist
        const { arr, ptr } = spec
        expect(arr).to.exist
        expect(ptr).to.exist
        expect(ptr).to.be.a("number")
        expect(arr.length).to.equal(3)
        freeFloatArray(instance, spec)
    })

    it('can fill a FloatArraySpec with probabilities', (): void => {

        const spec: FloatArraySpec = newFloatArray(instance, 52 * 4)
        const { arr } = spec
        expect(arr.length).to.equal(52 * 4)

        const init: GStateInit = instance.kRandomVal()
        const gstate: GState = new instance.GState(
            init,
            instance.GameVariant.STANDARD
        )
        const passOffset = gstate.passOffset()
        if (passOffset > 0) {
            for (let i = 0; i < 4; i++) {
                let hand = gstate.playersHand(i)
                let pass = instance.chooseThreeAtRandom(hand)
                gstate.setPassFor(i, pass)
            }
        }
        gstate.startGame()
        const card = new instance.Card(0)
        gstate.playCard(card)
        card.delete()

        fillProbabilities(spec, gstate)
        console.log('filled probabilties arr:', arr)
    })

    it('can fill a FloatArraySpec with an input tensor', (): void => {

        const spec: FloatArraySpec = newFloatArray(instance, 52 * 12)
        const { arr } = spec

        const init: GStateInit = instance.kRandomVal()
        const gstate: GState = new instance.GState(
            init,
            instance.GameVariant.STANDARD
        )
        const passOffset = gstate.passOffset()
        if (passOffset > 0) {
            for (let i = 0; i < 4; i++) {
                let hand = gstate.playersHand(i)
                let pass = instance.chooseThreeAtRandom(hand)
                gstate.setPassFor(i, pass)
            }
        }
        gstate.startGame()
        const card = new instance.Card(0)
        gstate.playCard(card)
        card.delete()

        asMin2022InputTensor(spec, gstate)

        // console.log('asMin2022InputTensor arr:')
        // for (let i = 0; i < 52; i++) {
        //     let a = i * 12
        //     let b = a + 12
        //     let sub = arr.subarray(a, b)
        //     console.log(sub)
        // }
    })


})
