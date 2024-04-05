import { createRequire } from 'module';
const require = createRequire(import.meta.url);

const fs = require('fs');
const wasmSource = new Uint8Array(fs.readFileSync("./T3/t3-1-c/mancalaBoard.wasm"));
const wasmModule = new WebAssembly.Module(wasmSource);
const wasmInstance = new WebAssembly.Instance(wasmModule, {
    env: {
        emscripten_resize_heap: function (requestedSize) {
            let oldSize = HEAPU8.length;
            requestedSize = requestedSize >>> 0;
            let maxHeapSize = 2147483648;
            if (requestedSize > maxHeapSize) {
                return false;
            }
            for (let cutDown = 1; cutDown <= 4; cutDown *= 2) {
                let overGrownHeapSize = oldSize * (1 + 0.2 / cutDown);
                overGrownHeapSize = Math.min(
                    overGrownHeapSize,
                    requestedSize + 100663296
                );
                let newSize = Math.min(
                    maxHeapSize,
                    alignUp(Math.max(requestedSize, overGrownHeapSize), 65536)
                );
                let replacement = emscripten_realloc_buffer(newSize);
                if (replacement) {
                    return true;
                }
            }
            return false;
        }
    }
});

function mancalaBoard(flag, seq, size) {
    // transform
    const savedStack = wasmInstance.exports.stackSave();
    const ptr = wasmInstance.exports.stackAlloc(seq.length * 4);
    const start = ptr >> 2;
    const heap = new Uint32Array(wasmInstance.exports.memory.buffer);

    for (let i = 0; i < seq.length; ++i) {
        heap[start + i] = seq[i];
    }

    const result = wasmInstance.exports.mancalaBoard(flag, ptr, size);
    const data = new Uint32Array(wasmInstance.exports.memory.buffer, result, 15);
    const ret = Array.from(data);

    wasmInstance.exports.stackRestore(savedStack);

    return ret;
}

console.log(mancalaBoard(1, [13, 16, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25, 24, 23, 15, 22, 12, 25, 11, 23, 12, 26], 34));