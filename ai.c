#include "ai.h"
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int ai_turn);


int ai_choose_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col)
{
    int bestScore = -1000;
    int bestMoveR = -1;
    int bestMoveC = -1;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {

            if (is_valid_move(board, r, c)) {

                make_move(board, r, c, AI_SYMBOL);

                // AI just moved, so it is now the player's turn
                int score = minimax(board, 0);

                // Undo move
                board[r][c] = EMPTY_CELL;

                if (score > bestScore) {
                    bestScore = score;
                    bestMoveR = r;
                    bestMoveC = c;
                }
            }
        }
    }

    if (bestMoveR != -1) {
        *row = bestMoveR;
        *col = bestMoveC;
        return 1;
    }

    return 0;
}

int minimax(char board[BOARD_SIZE][BOARD_SIZE], int ai_turn)
{
    if (check_winner(board, AI_SYMBOL)) {
        return 1;
    }

    if (check_winner(board, PLAYER_SYMBOL)) {
        return -1;
    }

    if (is_board_full(board)) {
        return 0;
    }

    if (ai_turn) {
        int bestScore = -1000;

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {

                if (is_valid_move(board, r, c)) {
                    make_move(board, r, c, AI_SYMBOL);

                    int score = minimax(board, 0);

                    board[r][c] = EMPTY_CELL;

                    if (score > bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;
    }
    else {
        int bestScore = 1000;

        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {

                if (is_valid_move(board, r, c)) {
                    make_move(board, r, c, PLAYER_SYMBOL);

                    int score = minimax(board, 1);

                    board[r][c] = EMPTY_CELL;

                    if (score < bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;
    }
}