#include <emscripten/emscripten.h>

#define _FORBIDDEN_SLOT(player) ((player) == 1 ? 13 : 6)
#define _SCORE_SLOT(player) ((player) == 1 ? 6 : 13)
#define _IS_MINE_SLOT(player, index) ((player) == 1 ? index <= 6 : index > 6)
#define _LEFT_SLOT(player) ((player) == 1 ? 0 : 7)
#define _RIGHT_SLOT(player) ((player) == 1 ? 5 : 12)
#define _OUTPUT_SLOT(slot) ((slot) < 6 ? (slot) + 11 : (slot) + 14)
#define _REMAIN(player, board) ((player) == 1 ? board[0] + board[1] + board[2] + board[3] + board[4] + board[5] : board[7] + board[8] + board[9] + board[10] + board[11] + board[12])

int _CONFIG_MAX_DEPTH = 18;

// 全局视角
int my_right, my_left, op_right, op_left, player, my_score_slot, op_score_slot;
// 公共临时变量
int eval, my_remain, op_remain, temp_get, temp_loss, temp_dup;
// 评估参数
int max_direct_get = 0, min_direct_loss = 65535, duplicate_move = 0;

// 我讨厌模拟
int simulateMove(int index, int* dest_board, int* res_board)
{
    int v = res_board[index];
    int forbidden_slot = index < 6 ? 13 : 6, player = index < 6 ? 1 : 2;
    for (unsigned int _ = 0; _ < 14; _++) {
        dest_board[_] = res_board[_];
    }
    dest_board[index] = 0;
    while (v--) {
        index = (index + 1) % 14;
        if (index != forbidden_slot) {
            dest_board[index] = res_board[index] + 1;
        } else {
            dest_board[index] = res_board[index];
            v++;
        }
    }
    if ((player == 1 && index == 6) || (player == 2 && index == 13)) {
        return 1;
    }
    if (dest_board[index] == 1 && ((player == 1 && index < 6) || (player == 2 && index > 6 && index != 13)) && dest_board[12 - index]) {
        dest_board[index < 6 ? 6 : 13] += dest_board[index] + dest_board[12 - index];
        dest_board[12 - index] = dest_board[index] = 0;
    }
    return 0;
}

int directGet(int* board)
{
    int board_copy[14], ret_loss = 0, temp_loss = 0;
    for (int i = op_right; i >= op_left; i--) {
        if (board[i]) {
            if (simulateMove(i, board_copy, board)) {
                temp_loss = 1 + directGet(board_copy);
            } else {
                temp_loss = board_copy[op_score_slot] - board[op_score_slot];
            }
            if (temp_loss > ret_loss) {
                ret_loss = temp_loss;
            }
        }
    }
    return ret_loss;
}

int evaluate(int* board)
{
    return board[my_score_slot] - board[op_score_slot];
}

// maxMin算法
int maxMinAlphaBeta(int* board, int alpha, int beta, char maximizing_flag, int max_depth, int* strategy_slot)
{
    if (max_depth == 0) {
        return evaluate(board);
    }
    my_remain = _REMAIN(player, board);
    op_remain = _REMAIN(player ^ 3, board);

    if (my_remain == 0 || op_remain == 0) {
        // 结局,结算
        board[_SCORE_SLOT(player)] += my_remain;
        board[_FORBIDDEN_SLOT(player)] += op_remain;
        board[0] = board[1] = board[2] = board[3] = board[4] = board[5] = 0;
        board[7] = board[8] = board[9] = board[10] = board[11] = board[12] = 0;
        return evaluate(board);
    }

    int board_copy[14], best_eval;

    if (maximizing_flag) {
        best_eval = -65536;
        // 我方行动
        for (int i = my_right; i >= my_left; i--) {
            if (board[i]) {
                if (simulateMove(i, board_copy, board))
                    // 连续行动
                    eval = maxMinAlphaBeta(board_copy, alpha, beta, 1, max_depth - 1, NULL);
                else
                    eval = maxMinAlphaBeta(board_copy, alpha, beta, 0, max_depth - 1, NULL);

                if (!strategy_slot) {
                    if (eval > best_eval) {
                        best_eval = eval;
                        // alpha-beta剪枝
                        if (alpha < best_eval) {
                            alpha = best_eval;
                            if (beta <= alpha) {
                                break;
                            }
                        }
                    }
                }
                // 顶层design, 优先再次行动或者取子或者防止取子
                else {
                    temp_get = board_copy[my_score_slot] - board[my_score_slot];
                    temp_dup = temp_get && (board_copy[(my_score_slot + 1) % 14] == board[(my_score_slot + 1) % 14]);
                    if (temp_dup)
                        temp_get += directGet(board_copy);
                    temp_loss = temp_dup ? 0 : directGet(board_copy);

                    if (eval > best_eval) {
                        best_eval = eval;
                        *strategy_slot = i;
                        if (alpha < best_eval) {
                            alpha = best_eval;
                        }
                    } else if (eval == best_eval) {
                        if (temp_get - temp_loss > max_direct_get) {
                            max_direct_get = temp_get - temp_loss;
                            *strategy_slot = i;
                        }
                    }
                }
            }
        }
    } else {
        best_eval = 65536;
        // 对手行动
        for (int i = op_right; i >= op_left; i--) {
            if (board[i]) {
                if (simulateMove(i, board_copy, board))
                    eval = maxMinAlphaBeta(board_copy, alpha, beta, 0, max_depth - 1, NULL);
                else
                    eval = maxMinAlphaBeta(board_copy, alpha, beta, 1, max_depth - 1, NULL);
                if (eval < best_eval) {
                    best_eval = eval;
                    if (beta > best_eval) {
                        beta = best_eval;
                        if (alpha >= beta) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return best_eval;
}

int mancalaOperator(int flag, int* board)
{
    // 初始化全局视角
    player = flag;
    my_right = _RIGHT_SLOT(player);
    my_left = _LEFT_SLOT(player);
    op_right = _RIGHT_SLOT(player ^ 3);
    op_left = _LEFT_SLOT(player ^ 3);
    my_score_slot = _SCORE_SLOT(player);
    op_score_slot = _FORBIDDEN_SLOT(player);
    // 初始化评估参数
    max_direct_get = 0;
    min_direct_loss = 0;
    duplicate_move = 0;
    // _CONFIG_MAX_DEPTH = max_depth;
    int ret_strategy_slot;
    maxMinAlphaBeta(board, -65536, 65536, 1, _CONFIG_MAX_DEPTH, &ret_strategy_slot);
    return _OUTPUT_SLOT(ret_strategy_slot);
}