#include "screen.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utf8.h"

void get_screen_dimensions(int *rows, int *cols) {
  struct winsize w;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  *rows = w.ws_row;
  *cols = w.ws_col;
}

void init_screen(screen_t *screen, uint32_t fill) {
  get_screen_dimensions(&screen->rows, &screen->cols);

  screen->cursorx = 0;
  screen->cursory = 0;

  screen->cells = malloc(screen->rows * screen->cols * sizeof *screen->cells);

  if (screen->cells == NULL) {
    exit(1);
  }

  screen_clear(screen, fill);
}

void draw(screen_t *target, screen_t *source) {
  int cursorAt = -1;
  char out[UTF8_LEN];
  
  int i;
  for (i = 0; i < source->rows*source->cols; ++i) {
    if (source->cells[i] != target->cells[i]) {
      if (cursorAt != i) {
        int y = (i / source->cols) + 1,
            x = (i % source->cols) + 1;

        printf("\033[%d;%dH", y, x);
        fflush(stdout);
        cursorAt = i;
      }
     
      target->cells[i] = source->cells[i];

      int len = pututf8(out, source->cells[i]);
      //puts(out);
      write(STDOUT_FILENO, out, len);
      cursorAt += 1;
    }
  }

  int cursorPos = source->cursory*source->cols + source->cursorx;

  target->cursorx = source->cursorx;
  target->cursory = source->cursory;

  if (cursorAt != cursorPos) {
    printf("\033[%d;%dH", source->cursory + 1, source->cursorx + 1);
    fflush(stdout);
  }
}

void screen_clear(screen_t *screen, uint32_t fill) {
  int i;
  for (i = 0; i < screen->cols * screen->rows; ++i) {
    screen->cells[i] = fill;
  }
}

