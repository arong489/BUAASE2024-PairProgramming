#include <emscripten/emscripten.h>
// #include <stdio.h>

EMSCRIPTEN_KEEPALIVE int bocchiShutUp(int flag, int* seq, int size)
{
    int ghost = 0, temp_max_cnt, count[6] = {};
    char multi = 0;
    for (unsigned int i = 0; i < size; i++) {
        count[seq[i] % 10 - 1] += seq[i] / 10 == flag;
    }
    temp_max_cnt = count[0];
    for (unsigned int i = 1; i < 6; i++) {
        if (temp_max_cnt < count[i]) {
            temp_max_cnt = count[i];
            ghost = i;
        } else if (temp_max_cnt == count[i]) {
            return 10;
        }
    }
    return ghost + 1 + flag * 10;
}
