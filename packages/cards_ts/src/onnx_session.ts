import ort from 'onnxruntime-node'
import type { GState } from '@playhearts/gstate_wasm'
import { asMin2022InputTensor, type FloatArraySpec } from './float_array.js'

export async function createSession(path: string): Promise<ort.InferenceSession> {
    return await ort.InferenceSession.create(path)
}

export async function runInferrence(session: ort.InferenceSession, gstate: GState, spec: FloatArraySpec): Promise<Float32Array> {

    asMin2022InputTensor(spec, gstate)
    console.log('filled Float32Array:', spec.arr)
    const inputTensor = new ort.Tensor('float32', spec.arr, [1, 52, 12])
    console.log('inputTensor:', inputTensor)
    const feeds: ort.InferenceSession.OnnxValueMapType = { input: inputTensor }
    // const fetches = ['output']
    try {
        // const options: ort.InferenceSession.RunOptions = { logSeverityLevel: 0 }
        console.log("running inference with inputTensor")
        const outputMap = await session.run(feeds)
        console.log('outputMap:', outputMap)
        return outputMap.output.data as Float32Array
    }
    catch (err) {
        console.error("Inference failed:", err.message)
        throw err
    }
}
