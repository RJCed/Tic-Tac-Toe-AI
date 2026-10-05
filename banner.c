/*
 * banner.c - big pixel-font title text.
 *
 * How it works:
 *   1. Each letter is a 5x7 grid of '#' (on) and '.' (off) - see GLYPHS.
 *   2. The letters are stamped onto a small "canvas" of pixels twice:
 *      first the shadow (shifted 1 pixel right and down), then the letters.
 *   3. Two pixel rows are packed into one terminal row using the
 *      half-block characters  ▀ (top)  ▄ (bottom)  █ (both).
 */

#include "banner.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define GLYPH_WIDTH     5
#define GLYPH_HEIGHT    7
#define LETTER_GAP      1                   /* blank pixels between letters */
#define WORD_GAP        3                   /* blank pixels between words   */
#define SHADOW_OFFSET   1                   /* shadow is shifted this far   */
#define PIXEL_ROWS      (BANNER_ROWS * 2)   /* two pixel rows per terminal row */
#define MAX_PIXEL_WIDTH 100

typedef struct {
    char letter;
    const char *rows[GLYPH_HEIGHT];
} Glyph;

/*
 * The letters this font can draw. Need another letter after renaming the
 * game? Add a new entry here (5 characters per row, 7 rows).
 */
static const Glyph GLYPHS[] = {
    {'A', {".###.", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"}},
    {'C', {".###.", "#...#", "#....", "#....", "#....", "#...#", ".###."}},
    {'D', {"####.", "#...#", "#...#", "#...#", "#...#", "#...#", "####."}},
    {'E', {"#####", "#....", "#....", "####.", "#....", "#....", "#####"}},
    {'I', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "#####"}},
    {'N', {"#...#", "##..#", "#.#.#", "#..##", "#...#", "#...#", "#...#"}},
    {'O', {".###.", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'R', {"####.", "#...#", "#...#", "####.", "#.#..", "#..#.", "#...#"}},
    {'S', {".####", "#....", "#....", ".###.", "....#", "....#", "####."}},
    {'T', {"#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."}},
    {'U', {"#...#", "#...#", "#...#", "#...#", "#...#", "#...#", ".###."}},
    {'W', {"#...#", "#...#", "#...#", "#.#.#", "#.#.#", "##.##", "#...#"}},
    {'Y', {"#...#", "#...#", ".#.#.", "..#..", "..#..", "..#..", "..#.."}},
    {'!', {"..#..", "..#..", "..#..", "..#..", "..#..", ".....", "..#.."}}
};

static const Glyph *find_glyph(char letter)
{
    char upper = (char)toupper((unsigned char)letter);

    for (size_t i = 0; i < sizeof GLYPHS / sizeof GLYPHS[0]; i++) {
        if (GLYPHS[i].letter == upper) {
            return &GLYPHS[i];
        }
    }
    return NULL; /* unknown letters are left blank */
}

/* Width in pixels of one word (letters plus the gaps between them). */
static int word_width(const char *word)
{
    int letters = (int)strlen(word);

    if (letters == 0) {
        return 0;
    }
    return letters * GLYPH_WIDTH + (letters - 1) * LETTER_GAP;
}

int banner_width(const BannerWord *words, int count)
{
    int width = 0;

    for (int i = 0; i < count; i++) {
        if (i > 0) {
            width += WORD_GAP;
        }
        width += word_width(words[i].word);
    }
    return width + SHADOW_OFFSET;
}

/*
 * Stamps a word onto the canvas starting at pixel column x.
 * `offset` shifts it right and down (used for the shadow).
 * With keep_existing set, pixels that are already colored are left alone.
 */
static void draw_word(unsigned char canvas[PIXEL_ROWS][MAX_PIXEL_WIDTH],
                      const char *word, int x, int offset, int color,
                      int keep_existing)
{
    for (const char *letter = word; *letter != '\0'; letter++) {
        const Glyph *glyph = find_glyph(*letter);

        if (glyph != NULL) {
            for (int row = 0; row < GLYPH_HEIGHT; row++) {
                for (int col = 0; col < GLYPH_WIDTH; col++) {
                    int px = x + col + offset;
                    int py = row + offset;

                    if (glyph->rows[row][col] != '#') {
                        continue;
                    }
                    if (px >= MAX_PIXEL_WIDTH || py >= PIXEL_ROWS) {
                        continue;
                    }
                    if (keep_existing && canvas[py][px] != 0) {
                        continue;
                    }
                    canvas[py][px] = (unsigned char)color;
                }
            }
        }
        x += GLYPH_WIDTH + LETTER_GAP;
    }
}

/* Appends text to the row; returns 0 if it would not fit. */
static int append(char *line, size_t *length, const char *text)
{
    size_t needed = strlen(text);

    if (*length + needed + 1 > BANNER_LINE_SIZE) {
        return 0;
    }
    memcpy(line + *length, text, needed + 1);
    *length += needed;
    return 1;
}

/* Turns two rows of canvas pixels into one terminal row of colored text. */
static int render_row(const unsigned char *top, const unsigned char *bottom,
                      int width, char *line)
{
    size_t length = 0;
    int current_fg = 0;   /* 0 = terminal default color */
    int current_bg = 0;
    int ok = 1;
    char code[32];

    line[0] = '\0';

    for (int x = 0; x < width; x++) {
        int upper = top[x];
        int lower = bottom[x];
        const char *symbol;
        int fg;
        int bg;

        if (upper == 0 && lower == 0) {
            symbol = " ";  fg = current_fg;  bg = 0;
        } else if (lower == 0) {
            symbol = "▀";  fg = upper;       bg = 0;
        } else if (upper == 0) {
            symbol = "▄";  fg = lower;       bg = 0;
        } else if (upper == lower) {
            symbol = "█";  fg = upper;       bg = 0;
        } else {
            symbol = "▀";  fg = upper;       bg = lower;
        }

        /* Only send a color code when the color actually changes. */
        if (bg != current_bg) {
            if (bg == 0) {
                snprintf(code, sizeof code, "\033[49m");
            } else {
                snprintf(code, sizeof code, "\033[48;5;%dm", bg);
            }
            ok &= append(line, &length, code);
            current_bg = bg;
        }
        if (fg != current_fg) {
            snprintf(code, sizeof code, "\033[38;5;%dm", fg);
            ok &= append(line, &length, code);
            current_fg = fg;
        }
        ok &= append(line, &length, symbol);
    }

    ok &= append(line, &length, "\033[0m");
    return ok;
}

int banner_render(const BannerWord *words, int count,
                    char lines[BANNER_ROWS][BANNER_LINE_SIZE])
{
    int width = banner_width(words, count);
    unsigned char canvas[PIXEL_ROWS][MAX_PIXEL_WIDTH] = {{0}};

    if (width > MAX_PIXEL_WIDTH) {
        return 0;
    }

    /* Pass 0 draws every shadow, pass 1 draws the letters on top. */
    for (int pass = 0; pass < 2; pass++) {
        int x = 0;

        for (int i = 0; i < count; i++) {
            if (pass == 0) {
                draw_word(canvas, words[i].word, x, SHADOW_OFFSET,
                        words[i].shadow_color, 1);
            } else {
                draw_word(canvas, words[i].word, x, 0,
                        words[i].main_color, 0);
            }
            x += word_width(words[i].word) + WORD_GAP;
        }
    }

    for (int row = 0; row < BANNER_ROWS; row++) {
        if (!render_row(canvas[row * 2], canvas[row * 2 + 1], width, lines[row])) {
            return 0;
        }
    }
    return 1;
}