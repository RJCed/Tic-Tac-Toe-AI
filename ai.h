#ifndef AI_H
#define AI_H

#include "game.h"

/*
 * Lets the AI (AI_SYMBOL, 'O') pick and play one move.
 *
 *   - Picks a square and places AI_SYMBOL on it.
 *   - Stores the chosen square in *row and *col so the caller can show it.
 *   - Returns 1 on success, or 0 if the board has no empty square.
 *
 * The rest of the program only depends on this function, so you can swap
 * the placeholder strategy in ai.c for Minimax without touching any
 * other file.
 */
int ai_move(char board[BOARD_SIZE][BOARD_SIZE], int *row, int *col);

#endif /* AI_H */