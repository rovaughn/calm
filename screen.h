#ifndef SCREEN_H
#define SCREEN_H
#include "buffer.h"

typedef struct {
  int   cursorx,
        cursory;
  int   rows,
        cols;
  char *cells;
} screen_t;

void get_screen_dimensions(int *rows, int *cols);
void init_screen(screen_t *screen, char fill);
void render(screen_t *screen);
void draw(screen_t *target, screen_t *source);

#endif
