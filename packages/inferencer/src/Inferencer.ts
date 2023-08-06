import { type FloatArraySpec, newFloatArray, asMin2022InputTensor, cardSetAsOrds, freeFloatArray } from '@playhearts/cards_ts'
import Socket from "./Socket.js"
import type { GState } from '@playhearts/gstate_wasm'
import { Buffer } from 'node:buffer'
import debug from 'debug'
import type * as gstate_wasm from '@playhearts/gstate_wasm'

const dlog = debug("Inferencer")

export interface SocketArgs {
    host: string,
    port: number,
}

export interface ChooseThreeArgs {
    instance: gstate_wasm.GStateModule,
    gstate: gstate_wasm.GState,
    player: number,
    bidSpec: FloatArraySpec,
}

async function createSocket(args: SocketArgs): Promise<Socket> {
    const socket = new Socket()
    const { host, port } = args
    await socket.connect(port, host)
    return socket
}

/// A class that manages a connection to a remote inference server.
/// The class is connected to one model on the server, and can (or will be)
/// able to run interences for any of our models.

export class Inferencer {
    private _socket: Socket
    private _model: string
    constructor() {
        this._socket = null
        this._model = null
    }

    async connectServer(args: SocketArgs): Promise<void> {
        if (this._socket != null) throw new Error("Server already connected")
        this._socket = await createSocket(args)
    }

    async disconnectServer(): Promise<void> {
        if (this._socket == null) throw new Error("Server already disconnected")
        await this._socket.end()
        this._socket = null
    }

    async connectModel(modelPath: string): Promise<void> {
        if (this._socket == null) throw new Error("Must connect to server before connecting to model")
        if (this._model != null) throw new Error("Model already connected")
        const modelSpec = `{ "model": "${modelPath}" }`
        dlog("Sending model spec:", modelSpec)
        await this._socket.write(Buffer.from(modelSpec))
        const chunk = await this._socket.read(2)
        const response = chunk.toString()
        if (response != "OK") throw new Error("Failed to connect to model")
        this._model = modelPath
    }

    async runInferrence(gstate: GState, spec: FloatArraySpec): Promise<Float32Array> {
        asMin2022InputTensor(spec, gstate)
        try {
            const expectedSize = 52 * 12 * 4
            const buffer: Buffer = Buffer.copyBytesFrom(spec.arr)
            if (buffer.byteLength != expectedSize) throw new Error(`Buffer size ${buffer.byteLength} is not expected size ${expectedSize}`)
            this._socket.write(buffer)

            const outputElements: number = 52
            const data: Float32Array = await this.receiveData(outputElements)
            return data
        }
        catch (err) {
            console.error("Inference failed:", err.message)
            throw err
        }
    }

    private async receiveData(size: number): Promise<Float32Array> {
        const expectedBytes: number = size * Float32Array.BYTES_PER_ELEMENT
        let buffer: Buffer = Buffer.alloc(0)
        while (true) {
            const needed = expectedBytes - buffer.length
            const chunk: Buffer = Buffer.from(await this._socket.read(needed))
            buffer = buffer == null ? chunk : Buffer.concat([buffer, chunk])
            if (buffer.length == expectedBytes) break
            else if (buffer.length > expectedBytes) throw new Error("got more data than expected!")
        }
        const result: Float32Array = new Float32Array(buffer.buffer, buffer.byteOffset, buffer.byteLength / Float32Array.BYTES_PER_ELEMENT)
        return result
    }

    async chooseThree(args: ChooseThreeArgs): Promise<void> {
        const { instance, gstate, player, bidSpec } = args
        const _hand: gstate_wasm.CardSet = gstate.playersHand(player)
        const hand: number[] = cardSetAsOrds(_hand)

        bidSpec.arr.fill(0.0)
        hand.forEach((card: number): void => { bidSpec.arr[card] = 1.0 })

        const outputData: Float32Array = await this.runBidInferrence(bidSpec)
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

    async runBidInferrence(spec: FloatArraySpec): Promise<Float32Array> {
        try {
            const expectedSize = 52 * 1 * Float32Array.BYTES_PER_ELEMENT
            const buffer: Buffer = Buffer.copyBytesFrom(spec.arr)
            if (buffer.byteLength != expectedSize) throw new Error(`Buffer size ${buffer.byteLength} is not expected size ${expectedSize}`)
            await this._socket.write(buffer)

            const outputElements: number = 52
            const data: Float32Array = await this.receiveData(outputElements)
            return data
        }
        catch (err) {
            console.error("Inference failed:", err.message)
            throw err
        }
    }
}
