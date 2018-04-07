#include "ncurses.h"

#include <ncurses.h>
#include <stdlib.h>

/**
 * Initialization of ncurses.
 */
void ncurses_initialize() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  refresh();
  curs_set(FALSE);
}

/**
 * Stop ncurses mode.
 */
void ncurses_stop() {
  endwin();
}

/**
 * Initialization of colors.
 */
void ncurses_colors() {
  /* Check colors support */
  if(has_colors() == FALSE) {
    ncurses_stop();
    fprintf(stderr, "No color support for this terminal.\n");
    exit(EXIT_FAILURE);
  }

  /* Activate colors */
  start_color();
}

/**
 * Check the sizes of the terminal.
 * @param height the height needed
 * @param width the width needed
 * @return TRUE if the sizes are OK else returns FALSE
 */
int ncurses_checksize(int height, int width) {
  int result = TRUE;

  if((COLS < width) || (LINES < height))
    result = FALSE;

  return result;
}
