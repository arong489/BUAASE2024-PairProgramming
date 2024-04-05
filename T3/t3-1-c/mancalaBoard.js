import { createRequire } from 'module';
const require = createRequire(import.meta.url);

const fs = require('fs');
const wasmSource = new Uint8Array(fs.readFileSync("./t3-1-c/mancalaBoard.wasm"));
const wasmModule = new WebAssembly.Module(wasmSource);
const wasmInstance = new WebAssembly.Instance(wasmModule, {
    env: {
    }
});

export default function mancalaBoard(flag, seq, size) {
    // transform
    const savedStack = wasmInstance.exports.stackSave();
    const ptr = wasmInstance.exports.stackAlloc(seq.length * 4);
    const start = ptr >> 2;
    const heap = new Uint32Array(wasmInstance.exports.memory.buffer);

    for (let i = 0; i < seq.length; ++i) {
        heap[start + i] = seq[i];
    }

    const result = wasmInstance.exports.mancalaBoard(flag, ptr, size);

    wasmInstance.exports.stackRestore(savedStack);

    return result;
}