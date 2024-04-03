#include <emscripten/emscripten.h>

#define _TO_BOARD(x) (((x) / 10 - 1) * 7 + (x) % 10 - 1)
#define _FORBIDDEN_SLOT(x) ((x) == 1 ? 13 : 6)
#define _SCORE_SLOT(x) ((x) == 1 ? 6 : 13)
#define _IS_MINE_SLOT(x, i) ((x) == 1 ? i <= 6 : i > 6)

EMSCRIPTEN_KEEPALIVE int mancalaResult(int flag, int* seq, int size)
{
    int first_move = flag;
    unsigned int board[14] = { 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0 };
    unsigned int value, index, forbidden;
    for (unsigned int i = 0; i < size; i++) {
        if (!(board[0] | board[1] | board[2] | board[3] | board[4] | board[5]) | !(board[7] | board[8] | board[9] | board[10] | board[11] | board[12])) {
            // 游戏结束违规
            return 30000 + i;
        }
        index = _TO_BOARD(seq[i]);
        if (seq[i] / 10 != flag || board[index] == 0)
            // 空格操作违规和棋手顺序违规
            return 30000 + i;
        // 模拟播棋
        value = board[index];
        board[index] = 0;
        forbidden = _FORBIDDEN_SLOT(flag);
        while (value--) {
            index = (index + 1) % 14;
            if (index != forbidden) {
                board[index]++;
            } else value++;
        }
        // 后效检测，再次行动
        if (index == _SCORE_SLOT(flag)) {
            continue;
        }
        // 后效检测，己方空格
        if (_IS_MINE_SLOT(flag, index) && board[index] == 1 && board[12 - index] != 0) {
            board[_SCORE_SLOT(flag)] += board[index] + board[12 - index];
            board[index] = 0;
            board[12 - index] = 0;
        }
        flag ^= 3;
    }

    if (!(board[0] | board[1] | board[2] | board[3] | board[4] | board[5]) | !(board[7] | board[8] | board[9] | board[10] | board[11] | board[12])) {
        // 游戏正好结束
        return (int)15000 + board[_SCORE_SLOT(first_move)] - board[_FORBIDDEN_SLOT(first_move)];
    }
    return (int)20000 + board[_SCORE_SLOT(first_move)];
}