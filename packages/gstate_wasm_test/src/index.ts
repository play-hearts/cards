import factory from "@playhearts/gstate_wasm";

import type { GStateModule } from "@playhearts/gstate_wasm";

export const instance: GStateModule = await factory();

console.log(instance);


