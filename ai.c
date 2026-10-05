/*
 * ai.c - the computer opponent.
 *
 * PLACEHOLDER: the AI currently just takes the first empty square, scanning
 * A1, B1, C1, A2, B2, C2, A3, B3, C3 (row by row, left to right).
 * There is no strategy here on purpose - Minimax is left for you to write.
 */

#include "ai.h"

/*
 * Decides WHERE the AI should play. It must not change the board.
 *
 * >>> This is the function to replace when you write Minimax. <<<
 * For example, add your own helper above it, such as
 *     static int minimax(char board[BOARD_SIZE][BOARD_SIZE], ...)
 * and call it from here to find the best row and column.
 * Whatever you do, finish by storing the answer in *row and *col and
 * returning 1 (or return 0 if there is no legal move).
 *
 * Current placeholder: scan the board and pick the first empty square.
 */
static int choose_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col)
{
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (is_valid_move(board, r, c)) {
                *row = r;
                *col = c;
                return 1;
            }
        }
    }
    return 0; /* board is full */
}

int ai_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col)
{
    int chosen_row;
    int chosen_col;

    if (!choose_move(board, &chosen_row, &chosen_col)) {
        return 0;
    }

    make_move(board, chosen_row, chosen_col, AI_SYMBOL);

    *row = chosen_row;
    *col = chosen_col;
    return 1;
}