#ifndef AI_H
#define AI_H

#include "game.h"

/*
 * Lets the AI (AI_SYMBOL, 'O') choose a move. It only DECIDES - it does not
 * place the piece. The caller (main.c) places it with make_move().
 *
 *   - Stores the chosen square in *row and *col.
 *   - Returns 1 on success, or 0 if the board has no empty square.
 *   - The board is left exactly as it was found. If you try moves on it
 *     (e.g. in Minimax), undo every one before returning.
 *
 * The rest of the program only depends on this function, so you can swap
 * the placeholder strategy in ai.c for Minimax without touching any
 * other file.
 */
int ai_choose_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col);
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int ai_turn);
int best_score(char board[BOARD_SIZE][BOARD_SIZE], int ai_turn);

#endif /* AI_H */