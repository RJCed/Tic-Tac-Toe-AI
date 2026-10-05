#include "ai.h"

int ai_choose_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col)
{
    int bestScore = -1000;
    int bestMoveR = -1;
    int bestMoveC = -1;

    // Check for each cell
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {

            if (is_valid_move(board, r, c)) {

                make_move(board, r, c, AI_SYMBOL);

                int score = minimax(board, 0);

                board[r][c] = EMPTY_CELL;

                if (score > bestScore) {
                    bestScore = score;
                    bestMoveR = r;
                    bestMoveC = c;
                }
            }
        }
    }

    if (bestMoveR == -1) {
        return 0;
    }

    *row = bestMoveR;
    *col = bestMoveC;

    return 1;
}

// Perform the minimax algorithm
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

    return best_score(board, ai_turn);
}


// Check for the best score 
int best_score(char board[BOARD_SIZE][BOARD_SIZE], int ai_turn)
{
    int bestScore = ai_turn ? -1000 : 1000;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {

            if (is_valid_move(board, r, c)) {

                char symbol = ai_turn ? AI_SYMBOL : PLAYER_SYMBOL;

                make_move(board, r, c, symbol);

                int score = minimax(board, !ai_turn);

                board[r][c] = EMPTY_CELL;

                if (ai_turn) {
                    if (score > bestScore) {
                        bestScore = score;
                    }
                }
                else {
                    if (score < bestScore) {
                        bestScore = score;
                    }
                }
            }
        }
    }

    return bestScore;
}