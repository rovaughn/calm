#ifndef SCREEN_H
#define SCREEN_H
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#include "buffer.h"

#define NCODES 2

typedef enum { BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE } stdcolor_t;

typedef union {
    unsigned rgb : 24;
    struct {
    uint8_t  b : 8,
             g : 8,
             r : 8;
    };
} color_t;

typedef struct {
    color_t fore;
    color_t back;
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
    int     x, y;
    bool    visible;
    style_t style;
} cursor_t;

typedef struct {
  int       rows,
            cols;
  cell_t   *cells;
  cursor_t  cursor;
} screen_t;

void fake_screen_init(screen_t *fake, int rows, int cols);
void fake_screen_reset(screen_t *fake, int rows, int cols);
void real_screen_init(buffer_t *buf, screen_t *real, int rows, int cols);
void real_screen_cleanup(buffer_t *buf, screen_t *real);
int read_dimensions(int *rows, int *cols);

void screen_flush(buffer_t *buf, screen_t *fake, screen_t *real);
void screen_free(screen_t *screen);

#endif
