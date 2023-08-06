import net from 'node:net'
import debug from 'debug'

const dlog = debug('Socket')

export default class Socket {
    private _socket: net.Socket

    constructor() {
        this._socket = new net.Socket()
    }

    public async connect(port: number, host: string): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this._socket.on('connect', () => {
                dlog(`Connected to ${host}:${port}`)
                resolve()
            })
            this._socket.on('error', (err) => {
                dlog(`Error connecting to ${host}:${port}: ${err.message}`)
                reject(err)
            })
            dlog(`Connecting to ${host}:${port}`)
            this._socket.connect(port, host)
        })
    }

    public async connectUnix(path: string): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this._socket.on('connect', () => {
                dlog(`Connected to ${path}`)
                resolve()
            })
            this._socket.on('error', (err) => {
                dlog(`Error connecting to ${path}: ${err.message}`)
                reject(err)
            })
            dlog(`Connecting to ${path}`)
            this._socket.connect(path)
        })
    }

    public async read(size: number, timeoutMs: number = 5000): Promise<Buffer> {
        return new Promise<Buffer>((resolve, reject) => {
            const t = setTimeout(() => {
                dlog(`Timeout reading ${size} bytes`)
                reject(new Error(`Timeout reading ${size} bytes`))
            }, timeoutMs)
            this._socket.on('error', (err) => {
                dlog(`Error reading ${size} bytes: ${err.message}`)
                reject(err)
            })
            this._socket.once('data', (data) => {
                clearTimeout(t)
                dlog(`Read ${data.byteLength} of ${size} bytes`)
                resolve(data)
            })
            dlog(`Reading ${size} bytes`)
            this._socket.read(size)
        })
    }

    public async write(buffer: Buffer): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this._socket.on('error', (err) => {
                dlog(`Error writing ${buffer.byteLength} bytes: ${err.message}`)
                reject(err)
            })
            dlog(`Writing ${buffer.byteLength} bytes`)
            this._socket.write(buffer, () => {
                dlog(`Wrote ${buffer.byteLength} bytes`)
                resolve()
            })
        })
    }

    public async end(): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            this._socket.on('error', (err) => {
                dlog(`Error ending socket: ${err.message}`)
                reject(err)
            })
            dlog(`Ending socket`)
            this._socket.end(() => {
                dlog(`Ended socket`)
                resolve()
            })
        })
    }
}
