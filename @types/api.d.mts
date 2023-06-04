/// <reference path="../@types/gstate_wasm.d.ts" />

declare module "@playhearts/gstate_wasm" {
import { GState, GStateModule } from "gstate_wasm";
export const instanceP: Promise<GStateModule>;
export function playOutGame(instance: GStateModule, gstate: GState): Promise<void>;
}
