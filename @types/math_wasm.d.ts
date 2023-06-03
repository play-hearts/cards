declare module 'math_wasm' {
    export interface Deletable {
        delete: () => void;
    }

    export interface Int126 {
        hi: BigInt;
        lo: BigInt;
    }

    export interface RandomGenerator extends Deletable {
        delete: () => null;
        randNorm: () => number;
    }

    export interface MathModule extends EmscriptenModule {
        RandomGenerator: (this: RandomGenerator) => void;
    }

    export interface MathModuleFactory extends EmscriptenModuleFactory<MathModule> {}
}
