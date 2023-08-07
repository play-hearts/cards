import net from 'node:net'
import debug from 'debug'
import { once } from 'node:events'
import { setTimeout } from 'node:timers/promises'
import util from 'node:util'

const dlog = debug('Socket')

export default class Socket {
    private _socket: net.Socket

    constructor() {
        this._socket = new net.Socket()
    }

    public async connect(port: number, host: string): Promise<void> {
        const connectPromise = once(this._socket, 'connect')
        this._socket.connect(port, host)
        try {
            await connectPromise
            dlog(`Connected to ${host}:${port}`)
        }
        catch (err) {
            dlog(`Error connecting to ${host}:${port}: ${err.message}`)
            throw err
        }
    }

    public async connectUnix(path: string): Promise<void> {
        const connectPromise = once(this._socket, 'connect')
        this._socket.connect(path)
        try {
            await connectPromise
            dlog(`Connected to ${path}`)
        }
        catch (err) {
            dlog(`Error connecting to ${path}: ${err.message}`)
            throw err
        }
    }

    public async read(size: number, timeoutMs: number = 20000): Promise<Buffer> {
        const dataPromise = once(this._socket, 'data')
        const timeoutPromise = setTimeout(timeoutMs, new Error("Timeout"))
        this._socket.read(size)
        const result = await Promise.race([dataPromise, timeoutPromise])
        if (result instanceof Error) throw result
        const [chunk] = result as [Buffer]
        return chunk
    }

    public async write(buffer: Buffer): Promise<void> {
        const write = util.promisify(this._socket.write).bind(this._socket)
        const timeoutPromise = setTimeout(100, new Error("Write Timeout"))
        const writePromise = write(buffer)
        const result = await Promise.race([writePromise, timeoutPromise])
        if (result instanceof Error) throw result
    }

    public async end(): Promise<void> {
        const closePromise = once(this._socket, 'close')

        dlog(`Closing socket`)
        this._socket.end()

        try {
            await closePromise
            dlog(`Socket closed`)
        }
        catch (err) {
            dlog(`Error closing socket: ${err.message}`)
            throw err
        }
    }
}
