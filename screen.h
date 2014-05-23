#ifndef SCREEN_H
#define SCREEN_H
#include "buffer.h"
#include <stdbool.h>

typedef struct {
  int       cursorx,
            cursory;
  bool      showcursor;
  int       rows,
            cols;
  uint32_t *cells;
} screen_t;

void get_screen_dimensions(int *rows, int *cols);
void init_screen(screen_t *screen, uint32_t fill);
void draw(screen_t *target, screen_t *source);
void screen_clear(screen_t *screen, uint32_t fill);
void screen_put(screen_t *screen, int x, int y, uint32_t c);

#endif
