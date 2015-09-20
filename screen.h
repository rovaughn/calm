#ifndef SCREEN_H
#define SCREEN_H
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#include "buffer.h"

#define NCODES 2

typedef enum { BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE } color_t;

typedef struct {
    color_t forecolor     : 3;
    bool    forebright    : 1;
    color_t backcolor     : 3;
    bool    backbright    : 1;
    bool    bold          : 1;
    bool    dim           : 1;
    bool    underline     : 1;
    bool    inverse       : 1;
    bool    invisible     : 1;
    bool    strikethrough : 1;
} style_t;

typedef struct {
  wchar_t codes[NCODES];
  style_t style;
} cell_t;

typedef struct {
  int     cursorx,
          cursory;
  bool    showcursor;
  int     rows,
          cols;
  cell_t *cells;
} screen_t;

void screen_clear(screen_t *screen);
void screen_init (buffer_t *buf, screen_t *fake, screen_t *real, int rows, int cols);
void screen_flush(buffer_t *buf, screen_t **fake, screen_t **real);
void screen_free(screen_t *screen);

#endif
