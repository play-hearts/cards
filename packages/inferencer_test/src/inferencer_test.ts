import factory, { Trick, TrickOrdRep } from '@playhearts/gstate_wasm'
import { type FloatArraySpec, newFloatArray, asMin2022InputTensor, cardSetAsOrds, freeFloatArray } from '@playhearts/cards_ts'
import Socket from "./Socket.js"
import type { GState } from '@playhearts/gstate_wasm'
import { Buffer } from 'node:buffer'
import { EventEmitter } from 'node:events'

import type * as gstate_wasm from '@playhearts/gstate_wasm'

import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
chai.use(chaiAsPromised)
const { expect } = chai

function createSocket(): Socket {
    return new Socket()
}

async function receiveData(playSession: Socket, size: number): Promise<Float32Array> {
    const expectedBytes: number = size * Float32Array.BYTES_PER_ELEMENT
    let buffer: Buffer = Buffer.alloc(0)
    while (true) {
        const needed = expectedBytes - buffer.length
        const chunk: Buffer = Buffer.from(await playSession.read(needed))
        buffer = buffer == null ? chunk : Buffer.concat([buffer, chunk])
        if (buffer.length == expectedBytes) break
        else if (buffer.length > expectedBytes) throw new Error("got more data than expected!")
    }
    const result: Float32Array = new Float32Array(buffer.buffer, buffer.byteOffset, buffer.byteLength / Float32Array.BYTES_PER_ELEMENT)
    expect(result.BYTES_PER_ELEMENT).to.equal(4)
    expect(result.length).to.equal(size)
    expect(result.byteLength).to.equal(expectedBytes)
    return result
}

export async function runBidInferrence(bidSession: Socket, spec: FloatArraySpec): Promise<Float32Array> {
    try {
        expect(spec.arr.BYTES_PER_ELEMENT).to.equal(4)
        const expectedSize = 52 * 1 * Float32Array.BYTES_PER_ELEMENT
        expect(spec.arr.byteLength).to.equal(expectedSize)
        const buffer: Buffer = Buffer.copyBytesFrom(spec.arr)
        if (buffer.byteLength != expectedSize) throw new Error(`Buffer size ${buffer.byteLength} is not expected size ${expectedSize}`)
        await bidSession.write(buffer)

        const outputElements: number = 52
        const data: Float32Array = await receiveData(bidSession, outputElements)
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

export async function runInferrence(playSession: Socket, gstate: GState, spec: FloatArraySpec): Promise<Float32Array> {

    asMin2022InputTensor(spec, gstate)
    try {
        const expectedSize = 52 * 12 * 4
        expect(spec.arr.BYTES_PER_ELEMENT).to.equal(4)
        expect(spec.arr.byteLength).to.equal(expectedSize)
        const buffer: Buffer = Buffer.copyBytesFrom(spec.arr)
        if (buffer.byteLength != expectedSize) throw new Error(`Buffer size ${buffer.byteLength} is not expected size ${expectedSize}`)
        playSession.write(buffer)

        const outputElements: number = 52
        const data: Float32Array = await receiveData(playSession, outputElements)
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
    const session = createSocket()
    await session.connect(51515, "ryley.lan")
    const modelSpec = `{ "model": "${modelPath}" }`
    console.log("Sending model spec:", modelSpec)
    await session.write(Buffer.from(modelSpec))
    const chunk = await session.read(2)
    const response = chunk.toString()
    if (response.length == 2 && response[0] == 'O' && response[1] == 'K') {
        return session
    }
    throw new Error("Failed to create session")
}

interface ChooseThreeArgs {
    instance: gstate_wasm.GStateModule,
    gstate: gstate_wasm.GState,
    bidSession: Socket,
    player: number,
    bidSpec: FloatArraySpec,
}

async function chooseThree(args: ChooseThreeArgs): Promise<void> {
    const { instance, gstate, bidSession, player, bidSpec } = args
    const _hand: gstate_wasm.CardSet = gstate.playersHand(player)
    const hand: number[] = cardSetAsOrds(_hand)

    bidSpec.arr.fill(0.0)
    hand.forEach((card: number): void => { bidSpec.arr[card] = 1.0 })

    const outputData: Float32Array = await runBidInferrence(bidSession, bidSpec)
    for (let c = 0; c < 52; c++) {
        if (!hand.includes(c)) outputData[c] = 999.0
    }
    const result: number[] = Array.from(outputData.keys())
        .sort((a: number, b: number): number => outputData[a] - outputData[b])
        .slice(0, 3)

    const pass: gstate_wasm.CardSet = new instance.CardSet()
    result.forEach((card: number): void => { pass.addOrd(card) })
    gstate.setPassFor(player, pass)
    pass.delete()
}

export async function playOutGame(instance: gstate_wasm.GStateModule, gstate: gstate_wasm.GState): Promise<void> {
    const passOffset = gstate.passOffset()
    if (passOffset > 0) {
        const bidSession: Socket = await createSession("/opt/pho/models/standard/bid/e112.pt")
        console.log("Pass offset:", passOffset)
        const bidSpec: FloatArraySpec = newFloatArray(instance, 52)
        for (let player = 0; player < 4; player++) {
            await chooseThree({ instance, gstate, bidSession, player, bidSpec })
        }
        freeFloatArray(instance, bidSpec)
    }
    gstate.startGame()

    const playSession: Socket = await createSession("/opt/pho/models/standard/play/e165.pt")
    const playSpec: FloatArraySpec = newFloatArray(instance, 52 * 12)

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

        const outputData: Float32Array = await runInferrence(playSession, gstate, playSpec)
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

    freeFloatArray(instance, playSpec)

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
    EventEmitter.setMaxListeners(0) // default is 10, which is too low for us
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
