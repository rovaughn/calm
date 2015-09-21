// ../screen.o ../buffer.o ../utf8.o ../wcwidth.o
#include "../screen.h"
#include "assert.h"
#include <ctype.h>

const char *checkCleared(screen_t *screen) {
    if (screen->cursor.x != 0) {
        return "Expected cursor.x = 0";
    } else if (screen->cursor.y != 0) {
        return "Expected cursor.y = 0";
    } else if (screen->cursor.style.fore.rgb != 0xffffff) {
        return "Expected cursor.style.fore.n = WHITE";
    } else if (screen->cursor.style.back.rgb != 0x000000) {
        return "Expected cursor.style.back.n = BALCK";
    }

    int i;
    for (i = 0; i < screen->rows * screen->cols; i++) {
        cell_t cell = screen->cells[i];

        if (cell.codes[0] != ' ') {
            return "Expected space";
        } else if (cell.style.fore.rgb != 0xffffff) {
            return "Expected white foreground";
        } else if (cell.style.back.rgb != 0x000000) {
            return "Expected black background";
        }
    }

    return NULL;
}

int main(void) {
    int rows = 25, cols = 80;
    buffer_t buf = buf_new(1);
    screen_t fake, real;

    {
        fake_screen_init(&fake, rows, cols);

        assertError("fake_screen_init clears fake", checkCleared(&fake));
        assertEq("fake->rows", fake.rows, 25);
        assertEq("fake->cols", fake.cols, 80);
    }

    {
        char exp[] = "\e[H\e[2J\e[0m\e[?25l";

        real_screen_init(&buf, &real, rows, cols);

        assertEqBuf("real_screen_init clears screen", buf.data, buf.used, exp, strlen(exp));
        assertEq("real->rows", real.rows, 25);
        assertEq("real->cols", real.cols, 80);
        assertError("real is cleared", checkCleared(&real));
    }

    {
        char exp[] = "";

        fake_screen_reset(&fake, rows, cols);
        buf.used = 0;
        screen_flush(&buf, &fake, &real);

        assertEqBuf("Nothing", buf.data, buf.used, exp, strlen(exp));
    }

    {
        fake_screen_reset(&fake, rows, cols);

        fake.cells[0].codes[0] = 'h';
        fake.cells[1].codes[0] = 'e';
        fake.cells[2].codes[0] = 'l';
        fake.cells[3].codes[0] = 'l';
        fake.cells[4].codes[0] = 'o';

        fake.cells[fake.cols+0].codes[0] = 'w';
        fake.cells[fake.cols+1].codes[0] = 'o';
        fake.cells[fake.cols+2].codes[0] = 'r';
        fake.cells[fake.cols+3].codes[0] = 'l';
        fake.cells[fake.cols+4].codes[0] = 'd';

        char exp[] = "hello\e[2;1Hworld";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);

        assertEqBuf("Writes multiple lines", buf.data, buf.used, exp, strlen(exp));
    }

    {
        fake_screen_reset(&fake, rows, cols);

        fake.cells[0].codes[0] = 'h';
        fake.cells[0].style.fore.rgb = 0x00ffff;
        fake.cells[1].codes[0] = 'e';
        fake.cells[2].codes[0] = 'l';
        fake.cells[3].codes[0] = 'l';
        fake.cells[4].codes[0] = 'o';

        fake.cells[fake.cols+0].codes[0] = 'W';
        fake.cells[fake.cols+1].codes[0] = 'o';
        fake.cells[fake.cols+1].style.fore.rgb = 0xff0000;
        fake.cells[fake.cols+1].style.back.rgb = 0xaabbcc;
        fake.cells[fake.cols+2].codes[0] = 'r';
        fake.cells[fake.cols+3].codes[0] = 'l';
        fake.cells[fake.cols+4].codes[0] = 'd';

        // TODO: A lot of common colors (e.g. black, white) could be optimized
        //       using normal standard colors, or 256 colors.
        char exp[] = "\e[1;1H\e[38;2;0;255;255mh\e[2;1H\e[38;2;255;255;255mW\e[38;2;255;0;0;48;2;170;187;204mo";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);

        assertEqBuf("Adds colors", buf.data, buf.used, exp, strlen(exp));
    }

    buf_free(&buf);
    screen_free(&fake);
    screen_free(&real);

    return 0;
}
