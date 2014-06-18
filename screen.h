#ifndef SCREEN_H
#define SCREEN_H
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>

#define NCODES 2

#define DEFAULT_STYLE (BACK_BLACK | WHITE)

typedef enum {
  BLACK       = 0x00, RED     , GREEN     , YELLOW     , BLUE     , MAGENTA     , CYAN     , WHITE,
  BRIGHT      = 0x08,
  BACK_BLACK  = 0x10, BACK_RED, BACK_GREEN, BACK_YELLOW, BACK_BLUE, BACK_MAGENTA, BACK_CYAN, BACK_WHITE,
  BACK_BRIGHT = 0x80
} color_t;

enum {
  BOLD          = 1 << 0,
  DIM           = 1 << 1,
  UNDERLINE     = 1 << 2,
  INVERSE       = 1 << 3,
  INVISIBLE     = 1 << 4,
  STRIKETHROUGH = 1 << 5
};

typedef struct {
  wchar_t  codes[NCODES];
  uint16_t style;
} cell_t;

typedef struct {
  int       cursorx,
            cursory;
  bool      showcursor;
  int       rows,
            cols;
  cell_t   *cells;
} screen_t;

void screen_init (screen_t *fake, screen_t *real);
void screen_flush(screen_t **fake, screen_t **real);

#endif
