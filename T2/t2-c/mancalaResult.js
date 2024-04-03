import { createRequire } from 'module';
const require = createRequire(import.meta.url);

const fs = require('fs');
const wasmSource = new Uint8Array(fs.readFileSync("./t2-c/mancalaResult.wasm"));
const wasmModule = new WebAssembly.Module(wasmSource);
const wasmInstance = new WebAssembly.Instance(wasmModule, {
    env: {
    }
});

export default function mancalaResult(flag, seq, len) {
    // transform
    const savedStack = wasmInstance.exports.stackSave();
    const ptr = wasmInstance.exports.stackAlloc(seq.length * 4);
    const start = ptr >> 2;
    const heap = new Uint32Array(wasmInstance.exports.memory.buffer);

    for (let i = 0; i < seq.length; ++i) {
        heap[start + i] = seq[i];
    }

    const result = wasmInstance.exports.mancalaResult(flag, ptr, len);

    wasmInstance.exports.stackRestore(savedStack);

    return result;
}