import { EventEmitter } from 'node:events'
import { Inferencer } from '@playhearts/inferencer'
import { type FloatArraySpec, newFloatArray, cardSetAsOrds, freeFloatArray } from '@playhearts/cards_ts'
import debug from 'debug'
import factory from '@playhearts/gstate_wasm'
import type * as gstate_wasm from '@playhearts/gstate_wasm'

const dlog = debug("InferencerTest")

export async function playOutGame(instance: gstate_wasm.GStateModule, gstate: gstate_wasm.GState): Promise<void> {
    const passOffset = gstate.passOffset()
    if (passOffset > 0) {
        dlog("Pass offset:", passOffset)

        const bidInferencer: Inferencer = new Inferencer()
        await bidInferencer.connectServer({ host: "ryley.lan", port: 51515 })
        await bidInferencer.connectModel("/opt/pho/models/standard/bid/e112.pt")

        const bidSpec: FloatArraySpec = newFloatArray(instance, 52)
        for (let player = 0; player < 4; player++) {
            await bidInferencer.chooseThree({ instance, gstate, player, bidSpec })
        }
        freeFloatArray(instance, bidSpec)
        await bidInferencer.disconnectServer()
    }
    gstate.startGame()

    const inferencer: Inferencer = new Inferencer()
    await inferencer.connectServer({ host: "ryley.lan", port: 51515 })
    await inferencer.connectModel("/opt/pho/models/standard/play/e165.pt")

    const playSpec: FloatArraySpec = newFloatArray(instance, 52 * 12)

    while (!gstate.done()) {
        const legal: gstate_wasm.CardSet = gstate.legalPlays()
        const legalOrds: number[] = cardSetAsOrds(legal)
        if (legalOrds.length == 1) {
            const bestCard = new instance.Card(legalOrds[0])
            gstate.playCard(bestCard)
            dlog('forced: ', instance.nameOfCard(bestCard))
            bestCard.delete()
            continue
        }

        const outputData: Float32Array = await inferencer.runInferrence(gstate, playSpec)

        let bestValue = 1e9
        let bestOrd: number

        legalOrds.forEach((ord: number): void => {
            let value = outputData[ord]
            if (value < bestValue) {
                bestValue = value
                bestOrd = ord
            }
        })

        const bestCard = new instance.Card(bestOrd)
        gstate.playCard(bestCard)
        bestCard.delete()
    }

    freeFloatArray(instance, playSpec)
    inferencer.disconnectServer()

    for (let p = 0; p < 4; ++p) {
        const outcome = gstate.getPlayerOutcome(p)
        dlog(
            `Player ${p} zms: ${outcome.zms}, winPts: ${outcome.winPts}`
        )
    }
}


async function main(concurrent: number): Promise<void> {
    EventEmitter.setMaxListeners(0)
    const instance = await factory()

    const promises: Promise<void>[] = []
    for (let i = 0; i < concurrent; ++i) {
        const init: gstate_wasm.GStateInit = instance.kRandomVal()
        dlog('kRandomVal:', init)
        const gstate: gstate_wasm.GState = new instance.GState(
            init,
            instance.GameVariant.STANDARD
        )

        const promise = playOutGame(instance, gstate).then(() => gstate.delete())
        promises.push(promise)
    }

    await Promise.all(promises)
}

const concurrent = process.argv.length <= 2 ? 10 : parseInt(process.argv[2])
console.log(`Running ${concurrent} concurrent games`)
await main(concurrent)
process.exit(0)
