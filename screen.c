#include "screen.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utf8.h"
#include "wcwidth.h"

void screen_init(screen_t *fake, screen_t *real) {
  fake->cursorx    = 0;
  fake->cursory    = 0;
  fake->showcursor = false;

  struct winsize ws;

  if (ioctl(0, TIOCGWINSZ, &ws) != 0) {
    fake->rows = real->rows = 25;
    fake->cols = real->rows = 80;
  } else {
    fake->rows = real->rows = ws.ws_row;
    fake->cols = real->cols = ws.ws_col;
  }

  fake->cells = calloc(fake->rows * fake->cols, sizeof *fake->cells);
  real->cells = calloc(real->rows * real->cols, sizeof *real->cells);

  int i;
  for (i = 0; i < fake->rows * fake->cols; ++i) {
    fake->cells[i].codes[0] = ' ';
    fake->cells[i].style    = DEFAULT_STYLE;
  }
}

// http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
void screen_flush(screen_t **fakeptr, screen_t **realptr) {
  screen_t *fake = *fakeptr,
           *real = *realptr;

  if (real->showcursor && !fake->showcursor) {
    printf("\e[?25h");
  } else if (!real->showcursor && fake->showcursor) {
    printf("\e[?25l");
  }

  int currentx = -1,
      currenty = -1;

  int x, y;
  for (y = 0; y < fake->rows; ++y) {
    for (x = 0; x < fake->cols; ++x) {
      int i = y*fake->cols + x;

      uint16_t style = fake->cells[i].style;

      if (style != real->cells[i].style) {
        uint8_t colors  = style & 0xff;
        //        effects = style >> 8;

        if (colors & BRIGHT) {
          printf("\e[%dm", 90 + (colors & 0x7));
        } else {
          printf("\e[%dm", 30 + (colors & 0x7));
        }

        if (colors & BACK_BRIGHT) {
          printf("\e[%dm", 100 + ((colors >> 4) & 0x7));
        } else {
          printf("\e[%dm", 100 + ((colors >> 4) & 0x7));
        }
      }

      if (memcmp(fake->cells[i].codes, real->cells[i].codes, NCODES) == 0) {
        continue;
      }

      if (x != currentx || y != currenty) {
        printf("\e[%d;%dH", y + 1, x + 1);
        currentx = x;
        currenty = y;
      }

      wchar_t *codes    = fake->cells[i].codes;
      char     codestr  [NCODES*UTF8_LEN + 1];
      int      codecols = mk_wcswidth(codes, NCODES),
               n        = putsutf8(codestr, fake->cells[i].codes, NCODES);

      codestr[n] = '\0';

      printf("%s", codestr);
      currentx += codecols;

      if (currentx >= fake->cols) {
        currentx  = 0;
        currenty += 1;
      }
    }
  }

  screen_t *swap = *realptr;

  *realptr = *fakeptr;
  *fakeptr = swap;
}

