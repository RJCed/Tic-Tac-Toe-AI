#ifndef UI_H
#define UI_H

#include "game.h"

/*
 * Shows the title screen and asks who plays first: 1 = the player, 2 = the AI.
 * Keeps asking until the answer is valid.
 *
 * Returns 1 and sets *player_first (1 = player, 0 = AI) on success.
 * Returns 0 if the player typed Q to quit or stdin reached end-of-file.
 */
int ui_prompt_first_player(int *player_first);

/*
 * Draws the game screen and asks the player for a move.
 * Keeps asking until the move is valid (and tells the player what was wrong).
 *
 * `notice` is an optional message shown above the prompt, e.g. "AI chose A2".
 * Pass "" or NULL for none.
 *
 * Returns 1 and sets *row / *col on success.
 * Returns 0 if the player typed Q to quit or stdin reached end-of-file.
 */
int ui_prompt_player_move(char board[BOARD_SIZE][BOARD_SIZE], const char *notice,
                          int *row, int *col);

/* Redraws the board with "AI is thinking..." and pauses briefly. */
void ui_show_ai_thinking(char board[BOARD_SIZE][BOARD_SIZE]);

/*
 * Shows the final board plus the win / lose / draw banner and asks whether
 * to play again. Returns 1 for "play again", 0 for "quit".
 */
int ui_show_game_over(char board[BOARD_SIZE][BOARD_SIZE], GameResult result);

/* Clears the screen and prints a short goodbye. */
void ui_show_goodbye(void);

#endif /* UI_H */