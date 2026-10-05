/*
 * ui.c - everything the player sees and types: drawing, centering, input.
 *
 * Output uses ANSI escape sequences (colors, clear screen), so it is meant
 * for a Linux terminal. Text is centered using the terminal's current width.
 */

/* Needed for nanosleep() and friends when compiling with -std=c17. */
#define _POSIX_C_SOURCE 200809L

#include "ui.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

/* ------------------------------------------------------------------ */
/*  ANSI escape sequences                                              */
/* ------------------------------------------------------------------ */

#define ANSI_CLEAR_SCREEN "\033[2J\033[H"
#define ANSI_RESET        "\033[0m"
#define ANSI_BOLD         "\033[1m"
#define ANSI_DIM          "\033[2m"
#define ANSI_REVERSE      "\033[7m"
#define ANSI_RED          "\033[1;31m"
#define ANSI_GREEN        "\033[1;32m"
#define ANSI_YELLOW       "\033[1;33m"
#define ANSI_CYAN         "\033[1;36m"

#define COLOR_PLAYER ANSI_CYAN
#define COLOR_AI     ANSI_RED

/* ------------------------------------------------------------------ */
/*  Layout constants                                                   */
/* ------------------------------------------------------------------ */

#define BANNER_WIDTH   38                  /* full width of a ╔═══╗ box        */
#define BANNER_INNER   (BANNER_WIDTH - 2)  /* space between the two ║ borders  */
#define BOARD_WIDTH    17                  /* 4-char row label + 13-char grid  */
#define LEGEND_WIDTH   10                  /* "You are: X"                     */
#define SCREEN_HEIGHT  22                  /* lines used by the tallest screen */

#define DEFAULT_COLUMNS 80                 /* used if the size is unknown      */
#define DEFAULT_ROWS    24

#define INPUT_BUFFER_SIZE 64
#define AI_THINK_DELAY_MS 800

/* ------------------------------------------------------------------ */
/*  Small drawing helpers                                              */
/* ------------------------------------------------------------------ */

/* Asks the terminal how big it is; falls back to 80x24 (e.g. when piped). */
static void get_terminal_size(int *columns, int *rows)
{
    struct winsize size;

    *columns = DEFAULT_COLUMNS;
    *rows = DEFAULT_ROWS;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
        if (size.ws_col > 0) {
            *columns = size.ws_col;
        }
        if (size.ws_row > 0) {
            *rows = size.ws_row;
        }
    }
}

/* How many spaces to print so that `content_width` columns end up centered. */
static int margin_for(int content_width)
{
    int columns, rows;
    get_terminal_size(&columns, &rows);

    int margin = (columns - content_width) / 2;
    return margin > 0 ? margin : 0;
}

static void print_spaces(int count)
{
    for (int i = 0; i < count; i++) {
        putchar(' ');
    }
}

/* Prints a (possibly multi-byte) string `count` times, e.g. "═" x 36. */
static void print_repeated(const char *text, int count)
{
    for (int i = 0; i < count; i++) {
        fputs(text, stdout);
    }
}

/* Prints plain ASCII `text` in the middle of the terminal. */
static void print_centered(const char *color, const char *text)
{
    print_spaces(margin_for((int)strlen(text)));
    printf("%s%s%s\n", color, text, ANSI_RESET);
}

/* Prints text lined up with the left edge of the title banner. */
static void print_left(const char *color, const char *text)
{
    print_spaces(margin_for(BANNER_WIDTH));
    printf("%s%s%s\n", color, text, ANSI_RESET);
}

/* Prints a message line, or just an empty line if there is no message. */
static void print_message_line(const char *color, const char *text)
{
    if (text == NULL || text[0] == '\0') {
        putchar('\n');
    } else {
        print_left(color, text);
    }
}

/* Prints a prompt (no newline) and flushes so it shows up before input. */
static void print_prompt(int margin, const char *text)
{
    print_spaces(margin);
    fputs(text, stdout);
    fflush(stdout);
}

static void pause_ms(int milliseconds)
{
    struct timespec duration;
    duration.tv_sec = milliseconds / 1000;
    duration.tv_nsec = (long)(milliseconds % 1000) * 1000000L;

    fflush(stdout);
    nanosleep(&duration, NULL);
}

/* Clears the terminal and adds blank lines on top to center vertically. */
static void begin_screen(void)
{
    int columns, rows;
    get_terminal_size(&columns, &rows);

    int top_padding = (rows - SCREEN_HEIGHT) / 2;

    fputs(ANSI_CLEAR_SCREEN, stdout);
    for (int i = 0; i < top_padding; i++) {
        putchar('\n');
    }
}

/* ------------------------------------------------------------------ */
/*  Boxes (title banner and result banner)                             */
/* ------------------------------------------------------------------ */

static void print_box_border(const char *color, const char *left, const char *right)
{
    print_spaces(margin_for(BANNER_WIDTH));
    fputs(color, stdout);
    fputs(left, stdout);
    print_repeated("═", BANNER_INNER);
    fputs(right, stdout);
    puts(ANSI_RESET);
}

/* One "║   text   ║" line with the text centered between the borders. */
static void print_box_text(const char *color, const char *text)
{
    int length = (int)strlen(text);
    int left_gap = (BANNER_INNER - length) / 2;
    int right_gap = BANNER_INNER - length - left_gap;

    print_spaces(margin_for(BANNER_WIDTH));
    fputs(color, stdout);
    fputs("║", stdout);
    print_spaces(left_gap);
    fputs(text, stdout);
    print_spaces(right_gap);
    fputs("║", stdout);
    puts(ANSI_RESET);
}

/* Draws a banner with one or two lines of text (pass NULL for no 2nd line). */
static void print_box(const char *color, const char *line1, const char *line2)
{
    print_box_border(color, "╔", "╗");
    print_box_text(color, line1);
    if (line2 != NULL) {
        print_box_text(color, line2);
    }
    print_box_border(color, "╚", "╝");
}

/* ------------------------------------------------------------------ */
/*  Board drawing                                                      */
/* ------------------------------------------------------------------ */

/* Draws one cell's three characters, e.g. " X ". */
static void print_cell(char value, int highlighted)
{
    if (value == PLAYER_SYMBOL || value == AI_SYMBOL) {
        const char *color = (value == PLAYER_SYMBOL) ? COLOR_PLAYER : COLOR_AI;
        printf("%s%s %c %s", color, highlighted ? ANSI_REVERSE : "", value, ANSI_RESET);
    } else {
        fputs("   ", stdout);
    }
}

/*
 * Draws the column letters and the box-drawing grid.
 * highlight[row][col] != 0 marks squares of the winning line.
 * Every line uses the same left margin so the grid stays aligned.
 */
static void draw_board(char board[BOARD_SIZE][BOARD_SIZE],
                       int highlight[BOARD_SIZE][BOARD_SIZE])
{
    int margin = margin_for(BOARD_WIDTH);

    print_spaces(margin);
    puts("      A   B   C");
    print_spaces(margin);
    puts("    ┌───┬───┬───┐");

    for (int row = 0; row < BOARD_SIZE; row++) {
        print_spaces(margin);
        printf("%d   │", row + 1);
        for (int col = 0; col < BOARD_SIZE; col++) {
            print_cell(board[row][col], highlight[row][col]);
            fputs("│", stdout);
        }
        putchar('\n');

        if (row < BOARD_SIZE - 1) {
            print_spaces(margin);
            puts("    ├───┼───┼───┤");
        }
    }

    print_spaces(margin);
    puts("    └───┴───┴───┘");
}

/* Marks the three squares of `player`'s winning line (if any) in the mask. */
static void mark_winning_cells(char board[BOARD_SIZE][BOARD_SIZE], char player,
                               int highlight[BOARD_SIZE][BOARD_SIZE])
{
    int cells[BOARD_SIZE][2];

    if (get_winning_line(board, player, cells)) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            highlight[cells[i][0]][cells[i][1]] = 1;
        }
    }
}

static void draw_legend(void)
{
    int margin = margin_for(LEGEND_WIDTH);

    print_spaces(margin);
    printf("You are: %s%c%s\n", COLOR_PLAYER, PLAYER_SYMBOL, ANSI_RESET);
    print_spaces(margin);
    printf("AI is:   %s%c%s\n", COLOR_AI, AI_SYMBOL, ANSI_RESET);
}

/* Clears the screen, then draws the title banner and the board. */
static void draw_game_screen(char board[BOARD_SIZE][BOARD_SIZE],
                             int highlight[BOARD_SIZE][BOARD_SIZE])
{
    begin_screen();
    print_box(ANSI_BOLD, "TIC TAC TOE", "VS AI");
    putchar('\n');
    draw_board(board, highlight);
    putchar('\n');
}

/* The screen shown while the game is running, down to the divider line. */
static void draw_play_screen(char board[BOARD_SIZE][BOARD_SIZE])
{
    int no_highlight[BOARD_SIZE][BOARD_SIZE] = {{0}};

    draw_game_screen(board, no_highlight);
    draw_legend();
    putchar('\n');

    print_spaces(margin_for(BANNER_WIDTH));
    fputs(ANSI_DIM, stdout);
    print_repeated("─", BANNER_WIDTH);
    puts(ANSI_RESET);
}

/* ------------------------------------------------------------------ */
/*  Input helpers                                                      */
/* ------------------------------------------------------------------ */

/*
 * Reads one line from stdin into `buffer` and removes the newline.
 * Returns 1 on success, 0 on end-of-file or error.
 *
 * If the line is too long for the buffer, the rest of it is thrown away
 * and the buffer is set to "" (which is invalid input), so leftover
 * characters never leak into the next prompt.
 */
static int read_line(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    size_t length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else if (!feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard the rest of the line */
        }
        buffer[0] = '\0';
    }

    return 1;
}

/* Returns 1 if the text contains nothing but whitespace. */
static int is_blank(const char *text)
{
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return 1;
}

/* Returns 1 if the line is just "Q" or "q" (spaces around it are fine). */
static int is_quit_command(const char *text)
{
    while (*text != '\0' && isspace((unsigned char)*text)) {
        text++;
    }
    if (toupper((unsigned char)*text) != 'Q') {
        return 0;
    }
    return is_blank(text + 1);
}

/* ------------------------------------------------------------------ */
/*  Public functions                                                   */
/* ------------------------------------------------------------------ */

int ui_prompt_player_move(char board[BOARD_SIZE][BOARD_SIZE], const char *notice,
                          int *row, int *col)
{
    char input[INPUT_BUFFER_SIZE];
    const char *error = NULL;

    for (;;) {
        draw_play_screen(board);

        /* One message line: an error if the last input was bad, else the notice. */
        if (error != NULL) {
            print_message_line(ANSI_YELLOW, error);
        } else {
            print_message_line(COLOR_AI, notice);
        }

        print_left(COLOR_PLAYER, "Your turn");
        print_prompt(margin_for(BANNER_WIDTH), "Enter a move (A1-C3, Q to quit): ");

        if (!read_line(input, sizeof input)) {
            return 0; /* end-of-file (Ctrl+D) */
        }
        if (is_quit_command(input)) {
            return 0;
        }

        if (!parse_move(input, row, col)) {
            error = "Invalid input. Try a move like B2.";
            continue;
        }
        if (!is_valid_move(board, *row, *col)) {
            error = "That square is already occupied.";
            continue;
        }

        return 1;
    }
}

void ui_show_ai_thinking(char board[BOARD_SIZE][BOARD_SIZE])
{
    draw_play_screen(board);
    putchar('\n'); /* keep the layout identical to the player's screen */
    print_left(COLOR_AI, "AI's turn");
    print_left(COLOR_AI, "AI is thinking...");

    pause_ms(AI_THINK_DELAY_MS);
}

int ui_show_game_over(char board[BOARD_SIZE][BOARD_SIZE], GameResult result)
{
    char input[INPUT_BUFFER_SIZE];
    int highlight[BOARD_SIZE][BOARD_SIZE] = {{0}};

    const char *color = ANSI_YELLOW;
    const char *headline = "DRAW";
    const char *message = "Nobody wins.";

    if (result == RESULT_PLAYER_WIN) {
        color = ANSI_GREEN;
        headline = "YOU WIN!";
        message = "Congratulations!";
        mark_winning_cells(board, PLAYER_SYMBOL, highlight);
    } else if (result == RESULT_AI_WIN) {
        color = ANSI_RED;
        headline = "AI WINS";
        message = "Better luck next time!";
        mark_winning_cells(board, AI_SYMBOL, highlight);
    }

    for (;;) {
        draw_game_screen(board, highlight);

        print_box(color, headline, NULL);
        putchar('\n');
        print_centered(ANSI_BOLD, message);
        putchar('\n');

        const char *prompt = "Press ENTER to play again, or Q to quit: ";
        print_prompt(margin_for((int)strlen(prompt)), prompt);

        if (!read_line(input, sizeof input)) {
            return 0; /* end-of-file */
        }
        if (is_quit_command(input)) {
            return 0;
        }
        if (is_blank(input)) {
            return 1;
        }
        /* Anything else: redraw and ask again. */
    }
}

void ui_show_goodbye(void)
{
    fputs(ANSI_CLEAR_SCREEN, stdout);
    putchar('\n');
    print_centered(ANSI_BOLD, "Thanks for playing!");
    putchar('\n');
}