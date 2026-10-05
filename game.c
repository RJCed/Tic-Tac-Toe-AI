/*
 * game.c - the rules of Tic-Tac-Toe. No printing and no input here.
 */

#include "game.h"

#include <ctype.h>
#include <string.h>

#define NUM_WIN_LINES 8

/*
 * Every way to win: 3 rows, 3 columns and 2 diagonals.
 * Each entry lists the three {row, column} squares that make up the line.
 */
static const int WIN_LINES[NUM_WIN_LINES][BOARD_SIZE][2] = {
    /* rows */
    {{0, 0}, {0, 1}, {0, 2}},
    {{1, 0}, {1, 1}, {1, 2}},
    {{2, 0}, {2, 1}, {2, 2}},
    /* columns */
    {{0, 0}, {1, 0}, {2, 0}},
    {{0, 1}, {1, 1}, {2, 1}},
    {{0, 2}, {1, 2}, {2, 2}},
    /* diagonals */
    {{0, 0}, {1, 1}, {2, 2}},
    {{0, 2}, {1, 1}, {2, 0}}
};

void initialize_board(char board[BOARD_SIZE][BOARD_SIZE])
{
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = EMPTY_CELL;
        }
    }
}

int is_valid_move(char board[BOARD_SIZE][BOARD_SIZE], int row, int col)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return 0;
    }
    return board[row][col] == EMPTY_CELL;
}

void make_move(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char player)
{
    board[row][col] = player;
}

int get_winning_line(char board[BOARD_SIZE][BOARD_SIZE], char player,
                    int cells[BOARD_SIZE][2])
{
    /* An "empty" line of three blanks must never count as a win. */
    if (player == EMPTY_CELL) {
        return 0;
    }

    for (int line = 0; line < NUM_WIN_LINES; line++) {
        int owned = 0;

        for (int i = 0; i < BOARD_SIZE; i++) {
            int row = WIN_LINES[line][i][0];
            int col = WIN_LINES[line][i][1];
            if (board[row][col] == player) {
                owned++;
            }
        }

        if (owned == BOARD_SIZE) {
            for (int i = 0; i < BOARD_SIZE; i++) {
                cells[i][0] = WIN_LINES[line][i][0];
                cells[i][1] = WIN_LINES[line][i][1];
            }
            return 1;
        }
    }

    return 0;
}

int check_winner(char board[BOARD_SIZE][BOARD_SIZE], char player)
{
    int cells[BOARD_SIZE][2];
    return get_winning_line(board, player, cells);
}

int is_board_full(char board[BOARD_SIZE][BOARD_SIZE])
{
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == EMPTY_CELL) {
                return 0;
            }
        }
    }
    return 1;
}

int parse_move(const char *text, int *row, int *col)
{
    /* Skip leading whitespace. */
    while (*text != '\0' && isspace((unsigned char)*text)) {
        text++;
    }

    /* Ignore trailing whitespace (including the newline from fgets). */
    size_t length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        length--;
    }

    /* A move is exactly two characters: a column letter, then a row digit. */
    if (length != 2) {
        return 0;
    }

    int column_index = toupper((unsigned char)text[0]) - 'A';
    int row_index    = text[1] - '1';

    if (column_index < 0 || column_index >= BOARD_SIZE ||
        row_index < 0    || row_index >= BOARD_SIZE) {
        return 0;
    }

    *row = row_index;
    *col = column_index;
    return 1;
}

void move_to_string(int row, int col, char text[MOVE_TEXT_SIZE])
{
    text[0] = (char)('A' + col);
    text[1] = (char)('1' + row);
    text[2] = '\0';
}