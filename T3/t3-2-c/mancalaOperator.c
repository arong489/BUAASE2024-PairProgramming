#include <emscripten/emscripten.h>

#define _FORBIDDEN_SLOT(player) ((player) == 1 ? 13 : 6)
#define _SCORE_SLOT(player) ((player) == 1 ? 6 : 13)
#define _IS_MINE_SLOT(player, index) ((player) == 1 ? index <= 6 : index > 6)
#define _LEFT_SLOT(player) ((player) == 1 ? 0 : 7)
#define _RIGHT_SLOT(player) ((player) == 1 ? 5 : 12)
#define _OUTPUT_SLOT(slot) ((slot) < 6 ? (slot) + 11 : (slot) + 14)
#define _REMAIN(player, board) ((player) == 1 ? board[0] + board[1] + board[2] + board[3] + board[4] + board[5] : board[7] + board[8] + board[9] + board[10] + board[11] + board[12])
#define _CONFIG_FINAL_PHASE_STEP 5
#define _CONFIG_FINAL_PHASE_CANCEL 4

// 我讨厌模拟
void simulateMove(int index, int* board)
{
    int v = board[index];
    int forbidden_slot = index < 6 ? 13 : 6;
    board[index] = 0;
    while (v--) {
        index = (index + 1) % 14;
        if (index != forbidden_slot) {
            board[index]++;
        } else {
            v++;
        }
    }
}

// 摸鱼评估.ing
int calculateDelay(int player, int* board)
{
    int times = 0;
    int my_right = _RIGHT_SLOT(player), my_left = _LEFT_SLOT(player);
    int board_copy[14];
    for (int i = 0; i < 14; i++)
        board_copy[i] = board[i];
    for (int i = my_right, check = 1; i >= my_left; i--, check++) {
        if (board_copy[i] <= check && board_copy[i]) {
            times += board_copy[i] != check;
            simulateMove(i, board_copy);
            i = my_right;
            check = 1;
        }
    }
    return times;
}

// 纯良无害的弱小函数
// int getScore(int player, int* board, int* max_index, char change_enable)
// {
//     //Note: 考虑再次行动取子
//     int my_right = _RIGHT_SLOT(player), my_left = _LEFT_SLOT(player);
//     int end_slot, score_slot = _SCORE_SLOT(player), forbidden_slot = _FORBIDDEN_SLOT(player);
//     int max_get = 0, temp_max_index;
//
//     for (int i = my_right; i >= my_left; i--) {
//
//         end_slot = (i + board[i]);
//         if (forbidden_slot > i && forbidden_slot <= end_slot)
//             end_slot++;
//         end_slot %= 14;
//
//         if (_IS_MINE_SLOT(player, end_slot) && end_slot != score_slot && board[end_slot] == 0 && board[12 - end_slot]) {
//             if (max_get < board[12 - end_slot] + 1) {
//                 max_get = board[12 - end_slot] + 1;
//                 temp_max_index = i;
//             }
//         }
//     }
//     if (max_index)
//         *max_index = temp_max_index;
//     if (change_enable) {
//         simulateMove(temp_max_index, board);
//     }
//     return max_get;
// }

//! 来点丝滑小连招, this is a super function
int getFinalScore(int player, int* board, int* strategy, int* combo, int* defense_slot, int* direct_get)
{
    int my_right = _RIGHT_SLOT(player), my_left = _LEFT_SLOT(player), score_slot = _SCORE_SLOT(player), forbidden_slot = _FORBIDDEN_SLOT(player);
    int end_slot;
    int max_get = 0, temp_strategy, temp_max_final_score = 0, temp_comb = 0, ret_comb = 0, temp_defense_slot = -1, temp_direct_get;
    //* 尝试直接取子
    for (int i = my_right; i >= my_left; i--) {
        if (board[i] == 0)
            continue;
        end_slot = (i + board[i]);
        if (forbidden_slot > i && forbidden_slot <= end_slot)
            end_slot++;
        end_slot %= 14;

        if (_IS_MINE_SLOT(player, end_slot) && end_slot != score_slot && board[end_slot] == 0 && board[12 - end_slot]) {
            if (max_get < board[12 - end_slot] + 1) {
                max_get = board[12 - end_slot] + 1;
                temp_strategy = i;
                temp_defense_slot = 12 - end_slot;
                temp_direct_get = board[12 - end_slot];
            }
            temp_comb = 1;
        }
    }

    int board_copy[14], loss_score;
    for (int i = 0; i < 14; i++)
        board_copy[i] = board[i];

    if (max_get)
        simulateMove(temp_strategy, board);

    for (int i = my_right, check = 1; i >= my_left; i--, check++) {
        //* 尝试再次行动
        if (board_copy[i] == check) {
            temp_max_final_score = 1;
            ret_comb++;
            simulateMove(i, board_copy);
            temp_max_final_score += getFinalScore(player, board_copy, strategy, &ret_comb, defense_slot, direct_get);
            if (temp_max_final_score > max_get) {
                max_get = temp_max_final_score;
                temp_strategy = i;
                temp_comb = ret_comb;
                for (int j = 0; j < 14; j++)
                    board[j] = board_copy[j];
            }
            break;
        }
    }

    if (strategy)
        *strategy = temp_strategy;
    if (combo)
        *combo += temp_comb;
    if (defense_slot)
        *defense_slot = temp_defense_slot;
    if (direct_get)
        *direct_get = temp_direct_get;

    return max_get;
}

int mancalaOperator(int player, int* board)
{
    int my_right = _RIGHT_SLOT(player), my_left = _LEFT_SLOT(player);
    int my_delay = calculateDelay(player, board), other_delay = calculateDelay(player ^ 3, board);
    int score_slot = _SCORE_SLOT(player), forbidden_slot = _FORBIDDEN_SLOT(player);
    //* 残局拖延
    if (my_delay - other_delay > _CONFIG_FINAL_PHASE_STEP) {
        for (int i = my_right, check = 1; i >= my_left; i--, check++) {
            if (board[i] < check && board[i]) {
                //* 残局防取子
                int board_copy[14], loss_score;
                for (int j = 0; j < 14; j++)
                    board_copy[j] = board[j];
                simulateMove(i, board_copy);
                loss_score = getFinalScore(player ^ 3, board_copy, NULL, NULL, NULL, NULL);
                if (loss_score && _REMAIN(player ^ 3, board_copy) + board_copy[forbidden_slot] > board_copy[score_slot] + _REMAIN(player, board_copy) + _CONFIG_FINAL_PHASE_CANCEL) {
                    break;
                } else {
                    return _OUTPUT_SLOT(i);
                }
            } else if (board[i] == check && board[i]) {
                return _OUTPUT_SLOT(i);
            }
        }
    }

    int strategy_slot, my_combo = 0, other_combo = 0, defense_slot, my_direct_get, other_direct_get;
    int board_copy[14];
    for (int i = 0; i < 14; i++)
        board_copy[i] = board[i];

    int max_get = getFinalScore(player, board_copy, &strategy_slot, &my_combo, NULL, &my_direct_get);
    int max_loss = getFinalScore(player ^ 3, board_copy, NULL, &other_combo, &defense_slot, &other_direct_get);
    if (my_combo > 1) {
        return _OUTPUT_SLOT(strategy_slot);
    }
    if (my_combo == 1) {
        if (my_direct_get == 0) {
            return _OUTPUT_SLOT(strategy_slot);
        }
        if (my_direct_get > other_direct_get || (my_direct_get == other_direct_get && player == 1)) {
            return _OUTPUT_SLOT(strategy_slot);
        } else {
            return _OUTPUT_SLOT(defense_slot);
        }
    }
    // combo为0
    // 直接防守
    if (defense_slot != -1) {
        return _OUTPUT_SLOT(defense_slot);
    }
    max_loss = 48;
    max_get = 0;
    int temp_loss, temp_get;
    // 不知道要干嘛的地方
    for (int i = my_right; i >= my_left; i--) {
        if (board[i]) {
            for (int j = 0; j < 14; j++)
                board_copy[j] = board[j];
            simulateMove(board_copy, strategy_slot);
            temp_loss = getFinalScore(player ^ 3, board_copy, NULL, NULL, &defense_slot, &other_direct_get);
            temp_get = getFinalScore(player, board_copy, &strategy_slot, &my_combo, NULL, &my_direct_get);

            if (max_loss < temp_loss) {
                max_loss = temp_loss;
                strategy_slot = i;
            } else if (max_loss == temp_loss)
            {
                strategy_slot = i;
            }
        }
    }
    return _OUTPUT_SLOT(strategy_slot);
}