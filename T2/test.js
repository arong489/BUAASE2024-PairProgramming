import assert from "assert";
import mancalaResult from "./t2-c/mancalaResult.js";
// Choose proper "import" depending on your PL.
// import { mancalaResult } from "./t2-as/build/release.js";
// import { mancala_result as mancalaResult } from "./t2_rust/pkg/t1_rust.js"
// [Write your own "import" for other PLs.]

assert.strictEqual(mancalaResult(1,[11,12],2),30001);
assert.strictEqual(mancalaResult(1,[14],1),20001);
assert.strictEqual(mancalaResult(1, [13, 16, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25, 24, 23, 15, 22, 12, 25, 11, 23, 12, 26], 34), 15008)
assert.strictEqual(mancalaResult(1, [13, 26, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25, 24, 23, 15, 22, 12, 25, 11, 23, 12, 26], 34), 30001)
assert.strictEqual(mancalaResult(1, [13, 13, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25, 24, 23, 15, 22, 12, 25, 11, 23, 12, 26], 34), 30001)
assert.strictEqual(mancalaResult(1, [13, 16, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25], 24), 20015)
assert.strictEqual(mancalaResult(1, [13, 16, 22, 21, 15, 24, 16, 12, 16, 11, 22, 15, 16, 14, 25, 15, 12, 26, 14, 15, 12, 23, 16, 25, 24, 23, 15, 22, 12, 25, 11, 23, 12, 26, 11], 35), 30034)

console.log("🎉 You have passed all the tests provided.");
