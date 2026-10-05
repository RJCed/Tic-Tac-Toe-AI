/*
 * main.c - the game loop. It only connects the other modules together:
 *   game.c  - rules (board, valid moves, winner)
 *   ai.c    - the computer opponent
 *   ui.c    - drawing and keyboard input
 */

#include <stdio.h>

#include "ai.h"
#include "game.h"
#include "ui.h"

/*
 * Plays one complete game.
 * Returns 1 if the player wants another game, 0 if they want to quit.
 */
static int play_round(void)
{
    char board[BOARD_SIZE][BOARD_SIZE];
    char notice[32] = "";               /* e.g. "AI chose A2", shown next turn */
    char move_text[MOVE_TEXT_SIZE];
    int row;
    int col;

    initialize_board(board);

    for (;;) {
        /* The player (X) always goes first: show the board, ask, validate. */
        if (!ui_prompt_player_move(board, notice, &row, &col)) {
            return 0; /* player quit */
        }
        make_move(board, row, col, PLAYER_SYMBOL);

        if (check_winner(board, PLAYER_SYMBOL)) {
            return ui_show_game_over(board, RESULT_PLAYER_WIN);
        }
        if (is_board_full(board)) {
            return ui_show_game_over(board, RESULT_DRAW);
        }

        /* Then the AI (O) moves. */
        ui_show_ai_thinking(board);
        if (!ai_move(board, &row, &col)) {
            return ui_show_game_over(board, RESULT_DRAW); /* no square left */
        }
        move_to_string(row, col, move_text);
        snprintf(notice, sizeof notice, "AI chose %s", move_text);

        if (check_winner(board, AI_SYMBOL)) {
            return ui_show_game_over(board, RESULT_AI_WIN);
        }
        if (is_board_full(board)) {
            return ui_show_game_over(board, RESULT_DRAW);
        }
    }
}

int main(void)
{
    while (play_round()) {
        /* play again */
    }

    ui_show_goodbye();
    return 0;
}