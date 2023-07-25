import type { GStateModule, GState } from '@playhearts/gstate_wasm'

// https://jott.live/markdown/wasm_vector_addition

export interface FloatArraySpec {
    arr: Float32Array,
    ptr: number
}

export function newFloatArray(instance: GStateModule, len: number): FloatArraySpec {
    const ptr: number = instance._malloc(len * 4)
    const arr: Float32Array = new Float32Array(instance.HEAPF32.buffer, ptr, len)
    if (arr.length != len) {
        throw new Error(`Float32Array allocation failed: ${arr.length} != ${len}`)
    }
    return { arr, ptr }
}

export function freeFloatArray(instance: GStateModule, spec: FloatArraySpec): void {
    instance._free(spec.ptr)
}

export function fillProbabilities(spec: FloatArraySpec, gstate: GState): void {
    const { arr, ptr } = spec
    if (arr.length != 52 * 4) {
        throw new Error("allocated array must be 52*4 long")
    }
    gstate.fillProbabilities(ptr)
}

export function asMin2022InputTensor(spec: FloatArraySpec, gstate: GState): void {
    const { arr, ptr } = spec
    if (arr.length != 52 * 12) {
        throw new Error("allocated array must be 52*12 long")
    }
    gstate.asMin2022InputTensor(ptr)
}

