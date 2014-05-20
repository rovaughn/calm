#include "screen.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void get_screen_dimensions(int *rows, int *cols) {
  struct winsize w;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  *rows = w.ws_row;
  *cols = w.ws_col;
}

void init_screen(screen_t *screen, char fill) {
  get_screen_dimensions(&screen->rows, &screen->cols);

  screen->cursorx = 0;
  screen->cursory = 0;

  screen->cells = malloc(screen->rows * screen->cols * sizeof *screen->cells);

  if (screen->cells == NULL) {
    exit(1);
  }

  memset(screen->cells, fill, screen->rows * screen->cols * sizeof *screen->cells);
}

void render(screen_t *screen) {
}

void draw(screen_t *target, screen_t *source) {
  int cursorAt = -1;
  
  int i;
  for (i = 0; i < source->rows*source->cols; ++i) {
    if (!isprint(source->cells[i])) {
      source->cells[i] = '?';
    }

    if (source->cells[i] != target->cells[i]) {
      if (cursorAt != i) {
        int y = (i / source->cols) + 1,
            x = (i % source->cols) + 1;

        printf("\033[%d;%dH", y, x);
        cursorAt = i;
      }
     
      target->cells[i] = source->cells[i];
      putchar(source->cells[i]);
      cursorAt += 1;
    }
  }

  int cursorPos = source->cursory*source->cols + source->cursorx;

  target->cursorx = source->cursorx;
  target->cursory = source->cursory;

  if (cursorAt != cursorPos) {
    printf("\033[%d;%dH", source->cursory + 1, source->cursorx + 1);
  }
}

