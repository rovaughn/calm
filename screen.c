#include "screen.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utf8.h"
#include "wcwidth.h"
#include <assert.h>

#define CSI "\x1b["

void fake_screen_init(screen_t *fake, int rows, int cols) {
    fake->rows = rows;
    fake->cols = cols;

    fake->cursor.x = 0;
    fake->cursor.y = 0;
    fake->cursor.visible = false;
    memset(&fake->cursor.style, 0, sizeof fake->cursor.style);
    fake->cursor.style.fore.rgb = 0xffffff;

    fake->cells = malloc(rows * cols * sizeof *fake->cells);

    memset(fake->cells, 0, fake->rows * fake->cols * sizeof *fake->cells);
    int i;
    for (i = 0; i < fake->rows * fake->cols; ++i) {
        fake->cells[i].codes[0] = ' ';
        fake->cells[i].style.fore.rgb = 0xffffff;
    }
}

void fake_screen_reset(screen_t *fake, int rows, int cols) {
    fake->cursor.x = 0;
    fake->cursor.y = 0;
    fake->cursor.visible = false;
    memset(&fake->cursor.style, 0, sizeof fake->cursor.style);
    fake->cursor.style.fore.rgb = 0xffffff;

    memset(fake->cells, 0, fake->rows * fake->cols * sizeof *fake->cells);
    int i;
    for (i = 0; i < fake->rows * fake->cols; ++i) {
        fake->cells[i].codes[0] = ' ';
        fake->cells[i].style.fore.rgb = 0xffffff;
    }
}

void real_screen_init(buffer_t *buf, screen_t *real, int rows, int cols) {
    buf_push_str(buf, CSI "H" CSI "2J" CSI "0m" CSI "?25l");

    real->cursor.x       = 0;
    real->cursor.y       = 0;
    real->cursor.visible = false;
    memset(&real->cursor.style, 0, sizeof real->cursor.style);
    real->cursor.style.fore.rgb = 0xffffff;

    real->rows = rows;
    real->cols = cols;

    real->cells = malloc(rows * cols * sizeof *real->cells);

    memset(real->cells, 0, real->rows * real->cols * sizeof *real->cells);
    int i;
    for (i = 0; i < real->rows * real->cols; i++) {
        real->cells[i].codes[0] = ' ';
        real->cells[i].style.fore.rgb = 0xffffff;
    }
}

void real_screen_cleanup(buffer_t *buf, screen_t *real) {
    buf_push_str(buf, CSI "0m" CSI "?25h");
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

static void move_cursor(buffer_t *buf, screen_t *real, int x, int y) {
    if (real->cursor.x == x && real->cursor.y == y) {
        return;
    }

    buf_push_str(buf, CSI);
    buf_push_int(buf, y + 1);
    buf_push_str(buf, ";");
    buf_push_int(buf, x + 1);
    buf_push_str(buf, "H");

    real->cursor.x = x;
    real->cursor.y = y;
}

static void write_cursor(buffer_t *buf, screen_t *real, wchar_t *codes, size_t ncodes) {
    int i = real->cursor.y*real->cols + real->cursor.x;
    wchar_t *realcodes = real->cells[i].codes;

    real->cells[i].style = real->cursor.style;

    memcpy(realcodes, codes, ncodes);
    buf_push_utf8(buf, codes, ncodes);
    real->cursor.x += mk_wcswidth(codes, ncodes);

    while (real->cursor.x > real->cols) {
        real->cursor.y += 1;
        real->cursor.x -= real->cols;
    }
}

static bool codes_eq(wchar_t *a, wchar_t *b, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) {
        if (a[i] == 0 && b[i] == 0) {
            return true;
        } else if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

void update_cell(buffer_t *buf, screen_t *fake, screen_t *real, int x, int y) {
    int i = y*fake->cols + x;

    cell_t *f = &fake->cells[i],
           *r = &real->cells[i];

    // SGR = Select Graphic Rendition
    unsigned sgrs[10] = {0};
    unsigned sgrn     = 0;

    bool foreDiffersReal   = f->style.fore.rgb != r->style.fore.rgb,
         backDiffersReal   = f->style.back.rgb != r->style.back.rgb,
         foreDiffersCursor = f->style.fore.rgb != real->cursor.style.fore.rgb,
         backDiffersCursor = f->style.back.rgb != real->cursor.style.back.rgb,
         textDiffers       = !codes_eq(f->codes, r->codes, NCODES);

    if ((foreDiffersReal || textDiffers) && foreDiffersCursor) {
        real->cursor.style.fore = f->style.fore;

        sgrs[sgrn++] = 38;
        sgrs[sgrn++] = 2;
        sgrs[sgrn++] = f->style.fore.r;
        sgrs[sgrn++] = f->style.fore.g;
        sgrs[sgrn++] = f->style.fore.b;
    }

    if ((backDiffersReal || textDiffers) && backDiffersCursor) {
        real->cursor.style.back = f->style.back;

        sgrs[sgrn++] = 48;
        sgrs[sgrn++] = 2;
        sgrs[sgrn++] = f->style.back.r;
        sgrs[sgrn++] = f->style.back.g;
        sgrs[sgrn++] = f->style.back.b;
    }

    assert(sgrn <= sizeof sgrs/sizeof *sgrs);

    if (sgrn > 0) {
        move_cursor(buf, real, x, y);

        buf_push_str(buf, CSI);
        buf_push_int(buf, sgrs[0]);

        unsigned i;
        for (i = 1; i < sgrn; i++) {
            buf_push_str(buf, ";");
            buf_push_int(buf, sgrs[i]);
        }

        buf_push_str(buf, "m");

        write_cursor(buf, real, f->codes, NCODES);
    } else if (foreDiffersReal || backDiffersReal || textDiffers) {
        move_cursor(buf, real, x, y);
        write_cursor(buf, real, f->codes, NCODES);
    }
}

// http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
void screen_flush(buffer_t *buf, screen_t *fake, screen_t *real) {
    if (fake->cursor.visible && !real->cursor.visible) {
        buf_push_str(buf, CSI "?25h");
    } else if (!fake->cursor.visible && real->cursor.visible) {
        buf_push_str(buf, CSI "?25l");
    }

    int x, y;
    for (y = 0; y < fake->rows; ++y) {
        for (x = 0; x < fake->cols; ++x) {
            update_cell(buf, fake, real, x, y);
        }
    }

    if (fake->cursor.visible) {
        move_cursor(buf, real, fake->cursor.x, fake->cursor.y);
    }
}

void screen_free(screen_t *screen) {
    free(screen->cells);
}
