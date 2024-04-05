#include <stdio.h>

#define _TO_BOARD(operation) (((operation) / 10 - 1) * 7 + (operation) % 10 - 1)
#define _FORBIDDEN_SLOT(flag) ((flag) == 1 ? 13 : 6)
#define _SCORE_SLOT(flag) ((flag) == 1 ? 6 : 13)
#define _IS_MINE_SLOT(flag, index) ((flag) == 1 ? index <= 6 : index > 6)
#define _IF_GAME_END(board) (!(board[0] | board[1] | board[2] | board[3] | board[4] | board[5]) | !(board[7] | board[8] | board[9] | board[10] | board[11] | board[12]))

#define __GAME_ENDED_ERROR 0xffffffff
#define __EMPTY_SLOT_ERROR 0xfffffffe
#define __PLAYER_ORDER_ERROR 0xfffffffd
#define __NORMAL 0

unsigned int chess_board[14] = { 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0 };
unsigned int very_first_player;
unsigned int next_operate_player;

void resetChess(unsigned int* board, unsigned int player)
{
    board[0] = board[1] = board[2] = board[3] = board[4] = board[5] = 4;
    board[7] = board[8] = board[9] = board[10] = board[11] = board[12] = 4;
    board[6] = board[13] = 0;
    very_first_player = next_operate_player = player;
}

unsigned int moveChess(unsigned int* board, int operation)
{
    if (_IF_GAME_END(board))
        // 游戏结束违规
        return __GAME_ENDED_ERROR;
    unsigned int index = _TO_BOARD(operation);
    if (operation / 10 != next_operate_player)
        // 和棋手顺序违规
        return __PLAYER_ORDER_ERROR;
    if (board[index] == 0)
        // 空格操作违规
        return __EMPTY_SLOT_ERROR;

    // 模拟播棋
    unsigned int value = board[index];
    board[index] = 0;
    unsigned int forbidden = _FORBIDDEN_SLOT(next_operate_player);
    while (value--) {
        index = (index + 1) % 14;
        if (index != forbidden) {
            board[index]++;
        } else
            value++;
    }

    if (_IF_GAME_END(board)) {
        // 游戏正好结束
        board[6] += board[0] + board[1] + board[2] + board[3] + board[4] + board[5];
        board[13] += board[7] + board[8] + board[9] + board[10] + board[11] + board[12];
        board[0] = board[1] = board[2] = board[3] = board[4] = board[5] = 0;
        board[7] = board[8] = board[9] = board[10] = board[11] = board[12] = 0;
        return __NORMAL;
    }
    // 后效检测，再次行动
    if (index == _SCORE_SLOT(next_operate_player)) {
        return __NORMAL;
    }
    // 后效检测，己方空格
    if (_IS_MINE_SLOT(next_operate_player, index) && board[index] == 1 && board[12 - index] != 0) {
        board[_SCORE_SLOT(next_operate_player)] += board[index] + board[12 - index];
        board[index] = 0;
        board[12 - index] = 0;
    }
    next_operate_player ^= 3;

    return __NORMAL;
}

void printBoard(unsigned int* board)
{
    printf("  |%2u|%2u|%2u|%2u|%2u|%2u|  \n", board[12], board[11], board[10], board[9], board[8], board[7]);
    printf("%2u|                 |%2u\n", board[13], board[6]);
    printf("  |%2u|%2u|%2u|%2u|%2u|%2u|  \n", board[0], board[1], board[2], board[3], board[4], board[5]);
}

int main(int argc, char const* argv[])
{
    resetChess(chess_board, 1);
    unsigned int num;
    char op;
    unsigned int operations[100] = {}, size;
    while (1) {
        do {
            op = getchar();
        } while (op == '\n');
        if (op == EOF) {
            break;
        }
        switch (op) {
        case 'p':
            printBoard(chess_board);
            break;
        case 'r':
            scanf("%u", &num);
            resetChess(chess_board, num);
            break;
        default:
            scanf("%u", &num);
            num += num < 6 ? 11 : 14;
            printf("time:%3u back: %u\n", size, moveChess(chess_board, num));
            printBoard(chess_board);
            operations[size++] = num;
            break;
        }
    }
    printf("\n%u, [%u", very_first_player, operations[0]);
    for (unsigned int i = 0; i < size; i++) {
        printf(", %u", operations[i]);
    }
    printf("], %u", size);
    return 0;
}
