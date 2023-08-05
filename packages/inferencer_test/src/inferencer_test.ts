import factory, { Trick, TrickOrdRep } from '@playhearts/gstate_wasm'
import { type FloatArraySpec, newFloatArray, asMin2022InputTensor } from '@playhearts/cards_ts'
import { createConnection, type Socket } from "node:net"
import type { GState } from '@playhearts/gstate_wasm'
import { Buffer } from 'node:buffer'

import type * as gstate_wasm from '@playhearts/gstate_wasm'

import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
chai.use(chaiAsPromised)
const { expect } = chai

async function receiveData(session: Socket, size: number): Promise<Float32Array> {
    const expectedBytes: number = size * Float32Array.BYTES_PER_ELEMENT
    return new Promise((resolve, reject) => {
        let partial: Buffer = null
        session.on('data', (chunk: Buffer) => {
            partial = partial == null ? chunk : Buffer.concat([partial, chunk])
            if (partial.length > expectedBytes) {
                reject(new Error("got more data than expected!"))
            } else if (partial.length == expectedBytes) {
                const result: Float32Array = new Float32Array(partial.buffer, partial.byteOffset, partial.byteLength / Float32Array.BYTES_PER_ELEMENT)
                expect(result.BYTES_PER_ELEMENT).to.equal(4)
                expect(result.length).to.equal(52)
                expect(result.byteLength).to.equal(expectedBytes)
                resolve(result)
            }
        })
        session.on('error', reject)
    })
}

export async function runInferrence(session: Socket, gstate: GState, spec: FloatArraySpec): Promise<Float32Array> {

    asMin2022InputTensor(spec, gstate)
    try {
        const expectedSize = 52 * 12 * 4
        expect(spec.arr.BYTES_PER_ELEMENT).to.equal(4)
        expect(spec.arr.byteLength).to.equal(expectedSize)
        const buffer: Buffer = Buffer.copyBytesFrom(spec.arr)
        if (buffer.byteLength != expectedSize) throw new Error(`Buffer size ${buffer.byteLength} is not expected size ${expectedSize}`)
        session.write(buffer)

        const outputElements: number = 52
        const data: Float32Array = await receiveData(session, outputElements)
        expect(data.BYTES_PER_ELEMENT).to.equal(4)
        expect(data.length).to.equal(52)
        expect(data.byteLength).to.equal(52 * 4)
        return data
    }
    catch (err) {
        console.error("Inference failed:", err.message)
        throw err
    }
}


async function createSession(modelPath: string): Promise<Socket> {
    return new Promise((resolve, reject) => {
        const session: Socket = createConnection(51515, "ryley.lan", async () => {
            session.on('data', (chunk: Buffer) => {
                const response = chunk.toString()
                if (response.length == 2 && response[0] == 'O' && response[1] == 'K') {
                    resolve(session)
                }
            })
            const modelSpec = `{ "model": "${modelPath}" }`
            console.log("Sending model spec:", modelSpec)
            session.write(modelSpec)
        })
    })
}

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

    const modelPath = "/opt/pho/models/standard/play/e165.pt"
    const session: Socket = await createSession(modelPath)
    const spec: FloatArraySpec = newFloatArray(instance, 52 * 12)

    while (!gstate.done()) {
        const p: number = gstate.currentPlayer()
        expect(p).to.be.within(0, 3)

        const legal: gstate_wasm.CardSet = gstate.legalPlays()
        const cardVec: gstate_wasm.CardVector = legal.asCardVector()
        if (cardVec.size() == 1) {
            const bestCard = cardVec.get(0)
            gstate.playCard(bestCard)
            console.log('forced: ', instance.nameOfCard(bestCard))
            bestCard.delete()
            cardVec.delete()
            legal.delete()
            continue
        }

        const outputData: Float32Array = await runInferrence(session, gstate, spec)
        expect(outputData).to.exist
        expect(outputData.BYTES_PER_ELEMENT).to.equal(4)
        expect(outputData.length).to.equal(52)
        expect(outputData.byteLength).to.equal(52 * 4)
        // console.log(outputData)

        let bestValue = 1e9
        let bestOrd: number

        for (let i = 0; i < cardVec.size(); ++i) {
            let card = cardVec.get(i)
            let value = outputData[card.ord()]
            if (value < bestValue) {
                bestValue = value
                bestOrd = card.ord()
            }
            card.delete()
        }

        const bestCard = new instance.Card(bestOrd)
        console.log('choice: ', instance.nameOfCard(bestCard))
        gstate.playCard(bestCard)
        bestCard.delete()
        cardVec.delete()
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

    await playOutGame(instance, gstate)

    gstate.delete()
}

await main()
process.exit(0)
