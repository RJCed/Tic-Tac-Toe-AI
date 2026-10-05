#ifndef BANNER_H
#define BANNER_H

/*
 * Big pixel-font text with a drop shadow, drawn with half-block characters
 * (▀ ▄ █) so every "pixel" looks square in the terminal.
 *
 * This module only builds the colored text rows. It knows nothing about
 * terminal size or centering - ui.c does that.
 */

#define BANNER_ROWS      4      /* terminal rows a banner occupies          */
#define BANNER_LINE_SIZE 2048   /* buffer size for one rendered banner row  */

typedef struct {
    const char *word;   /* letters to draw, e.g. "TIC" (see GLYPHS in banner.c) */
    int main_color;     /* 256-color palette index of the letters               */
    int shadow_color;   /* 256-color palette index of the drop shadow           */
} BannerWord;

/* Width in terminal columns of the words drawn side by side. */
int banner_width(const BannerWord *words, int count);

/*
 * Renders the words into BANNER_ROWS strings (colors included).
 * Every row has the same visible width, banner_width().
 * Returns 1 on success, 0 if it does not fit (nothing usable in `lines`).
 */
int banner_render(const BannerWord *words, int count,
                    char lines[BANNER_ROWS][BANNER_LINE_SIZE]);

#endif /* BANNER_H */