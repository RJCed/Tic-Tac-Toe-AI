#ifndef GAME_H
#define GAME_H

/* ------------------------------------------------------------------ */
/*  Constants                                                          */
/* ------------------------------------------------------------------ */

#define BOARD_SIZE 3

#define PLAYER_SYMBOL 'X'
#define AI_SYMBOL     'O'
#define EMPTY_CELL    ' '

/* Room for a move written as text, e.g. "A1", plus the '\0' terminator. */
#define MOVE_TEXT_SIZE 3

typedef enum {
    RESULT_PLAYER_WIN,
    RESULT_AI_WIN,
    RESULT_DRAW
} GameResult;

/* ------------------------------------------------------------------ */
/*  Board functions                                                    */
/* ------------------------------------------------------------------ */

/* Fills every square with EMPTY_CELL. */
void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]);

/* Returns 1 if (row, col) is on the board AND the square is empty, else 0. */
int is_valid_move(char board[BOARD_SIZE][BOARD_SIZE], int row, int col);

/* Places `player` on (row, col). Call is_valid_move() first. */
void make_move(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char player);

/* Returns 1 if `player` has three in a row (row, column or diagonal). */
int check_winner(char board[BOARD_SIZE][BOARD_SIZE], char player);

/*
 * Like check_winner(), but also reports WHICH three squares won.
 * On success returns 1 and fills cells[i][0] = row, cells[i][1] = column.
 * The UI uses this to highlight the winning line.
 */
int get_winning_line(char board[BOARD_SIZE][BOARD_SIZE], char player,
                    int cells[BOARD_SIZE][2]);

/* Returns 1 if no empty squares remain, else 0. */
int is_board_full(char board[BOARD_SIZE][BOARD_SIZE]);

/* ------------------------------------------------------------------ */
/*  Coordinate helpers ("B2" <-> board[1][1])                          */
/* ------------------------------------------------------------------ */

/*
 * Converts text such as "B2" or " b2 " into array indices.
 * Letter = column (A-C), digit = row (1-3).  Case does not matter and
 * surrounding whitespace is ignored.
 * Returns 1 and sets *row / *col on success; returns 0 for anything else.
 */
int parse_move(const char *text, int *row, int *col);

/* Converts array indices back to text, e.g. (1, 1) -> "B2". */
void move_to_string(int row, int col, char text[MOVE_TEXT_SIZE]);

#endif /* GAME_H */