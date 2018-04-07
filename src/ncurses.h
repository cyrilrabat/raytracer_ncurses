#ifndef _NCURSES_
#define _NCURSES_

/**
 * Initialization of ncurses.
 */
void ncurses_initialize();

/**
 * Stop ncurses mode.
 */
void ncurses_stop();

/**
 * Initialization of colors.
 */
void ncurses_colors();

/**
 * Check the sizes of the terminal.
 * @param height the height needed
 * @param width the width needed
 * @return TRUE if the sizes are OK else returns FALSE
 */
int ncurses_checksize(int height, int width);

#endif
