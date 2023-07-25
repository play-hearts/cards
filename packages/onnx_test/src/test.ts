import factory, { Trick, TrickOrdRep } from '@playhearts/gstate_wasm'
import { type FloatArraySpec, createSession, newFloatArray, runInferrence } from '@playhearts/cards_ts'

import type * as gstate_wasm from '@playhearts/gstate_wasm'

import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
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

    const session = await createSession('./model.onnx')
    const { inputNames, outputNames } = session
    console.log('inputNames: ', inputNames)
    console.log('outputNames: ', outputNames)
    const spec: FloatArraySpec = newFloatArray(instance, 52 * 12)

    while (!gstate.done()) {
        const legal: gstate_wasm.CardSet = gstate.legalPlays()
        const card: gstate_wasm.Card = instance.aCardAtRandom(legal)
        const p: number = gstate.currentPlayer()
        expect(p).to.be.within(0, 3)

        const outputData: Float32Array = await runInferrence(session, gstate, spec)
        expect(outputData).to.exist
        console.log(outputData)

        gstate.playCard(card)
        card.delete()
        legal.delete()
        const trick: Trick = gstate.currentTrick()
        expect(trick).to.not.be.undefined
        const trickArr: TrickOrdRep = trick.ordRep()
        for (let i = 0; i < 4; ++i) {
            const card: gstate_wasm.Card = trick.at(i)
            expect(card.ord()).to.equal(trickArr[i])
            // console.log(`Player ${i} played ${instance.nameOfCard(card)}`);
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

async function main(): Promise<void> {
    const instance = await factory()


    const init: gstate_wasm.GStateInit = instance.kRandomVal()
    console.log('kRandomVal:', init)
    const gstate: gstate_wasm.GState = new instance.GState(
        init,
        instance.GameVariant.STANDARD
    )


    playOutGame(instance, gstate)

}

main()
