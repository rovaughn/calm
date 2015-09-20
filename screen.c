#include "screen.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utf8.h"
#include "wcwidth.h"

#define CSI "\x1b["

void screen_clear(screen_t *screen) {
    memset(screen->cells, 0, screen->rows * screen->cols * sizeof *screen->cells);
    int i;
    for (i = 0; i < screen->rows * screen->cols; ++i) {
        screen->cells[i].codes[0] = ' ';
        screen->cells[i].style.forecolor = WHITE;
    }
}

int read_dimensions(int *rows, int *cols) {
    struct winsize ws;
    int err;
    
    if ((err = ioctl(0, TIOCGWINSZ, &ws)) != 0) {
        return err;
    }

    *rows = ws.ws_row;
    *cols = ws.ws_col;

    return 0;
}

void screen_init(buffer_t *buf, screen_t *fake, screen_t *real, int rows, int cols) {
    fake->cursorx    = 0;
    fake->cursory    = 0;
    fake->showcursor = false;

    buf_push_str(buf, CSI "H" CSI "2J");

    fake->rows = real->rows = rows;
    fake->cols = real->cols = cols;

    fake->cells = malloc(rows * cols * sizeof *fake->cells);
    real->cells = malloc(rows * cols * sizeof *real->cells);

    screen_clear(fake);
    screen_clear(real);
}

static void move_cursor(buffer_t *buf, int x, int y) {
    buf_push_str(buf, CSI);
    buf_push_int(buf, y + 1);
    buf_push_str(buf, ";");
    buf_push_int(buf, x + 1);
    buf_push_str(buf, "H");
}

// http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
void screen_flush(buffer_t *buf, screen_t **fakeptr, screen_t **realptr) {
    screen_t *fake = *fakeptr,
             *real = *realptr;

    /*if (real->showcursor && !fake->showcursor) {
    printf("\e[?25h");
    } else if (!real->showcursor && fake->showcursor) {
    printf("\e[?25l");
    }*/

    int currentx = 0,
        currenty = 0;

    int x, y;
    for (y = 0; y < fake->rows; ++y) {
        for (x = 0; x < fake->cols; ++x) {
            int i = y*fake->cols + x;

            style_t fstyle = fake->cells[i].style,
                    rstyle = real->cells[i].style;

            if (fstyle.forebright != rstyle.forebright ||
                fstyle.forecolor  != rstyle.forecolor) {
                if (x != currentx || y != currenty) {
                    move_cursor(buf, x, y);
                    currentx = x;
                    currenty = y;
                }

                if (fstyle.forebright) {
                    buf_push_str(buf, CSI);
                    buf_push_int(buf, 90 + fstyle.forecolor);
                    buf_push_str(buf, "m");
                } else {
                    buf_push_str(buf, CSI);
                    buf_push_int(buf, 30 + fstyle.forecolor);
                    buf_push_str(buf, "m");
                }
            }

            if (fstyle.backbright != rstyle.backbright ||
                fstyle.backcolor  != rstyle.backcolor) {
                if (x != currentx || y != currenty) {
                    move_cursor(buf, x, y);
                    currentx = x;
                    currenty = y;
                }

                if (fstyle.backbright) {
                    buf_push_str(buf, CSI);
                    buf_push_int(buf, 100 + fstyle.forecolor);
                    buf_push_str(buf, "m");
                } else {
                    buf_push_str(buf, CSI);
                    buf_push_int(buf, 40 + fstyle.forecolor);
                    buf_push_str(buf, "m");
                }
            }

            if (memcmp(fake->cells[i].codes, real->cells[i].codes, NCODES) != 0) {
                if (x != currentx || y != currenty) {
                    move_cursor(buf, x, y);
                    currentx = x;
                    currenty = y;
                }

                wchar_t *codes    = fake->cells[i].codes;
                char     codestr  [NCODES*UTF8_LEN];
                int      codecols = mk_wcswidth(codes, NCODES),
                         n        = putsutf8(codestr, fake->cells[i].codes, NCODES);

                buf_push(buf, codestr, n);
                currentx += codecols;

                if (currentx >= fake->cols) {
                    currentx  = 0;
                    currenty += 1;
                }
            }
        }
    }

    screen_t *swap = *realptr;

    *realptr = *fakeptr;
    *fakeptr = swap;

    screen_clear(*fakeptr);
}

void screen_free(screen_t *screen) {
    free(screen->cells);
}
