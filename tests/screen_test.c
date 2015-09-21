// ../screen.o ../buffer.o ../utf8.o ../wcwidth.o
#include "../screen.h"
#include "assert.h"
#include <ctype.h>

const char *checkCleared(screen_t *screen) {
    if (screen->cursor.x != 0) {
        return "Expected cursor.x = 0";
    } else if (screen->cursor.y != 0) {
        return "Expected cursor.y = 0";
    } else if (screen->cursor.style.forecolor != WHITE) {
        return "Expected cursor.style.forecolor = WHITE";
    } else if (screen->cursor.style.backcolor != BLACK) {
        return "Expected cursor.style.backcolor = BALCK";
    }

    int i;
    for (i = 0; i < screen->rows * screen->cols; i++) {
        cell_t cell = screen->cells[i];

        if (cell.codes[0] != ' ') {
            return "Expected space";
        } else if (cell.style.forecolor != WHITE) {
            return "Expected white foreground";
        } else if (cell.style.backcolor != BLACK) {
            return "Expected black background";
        }
    }

    return NULL;
}

void show(buffer_t buf) {
    size_t i;
    for (i = 0; i < buf.used; i++) {
        char c = buf.data[i];

        if (c == '\\') {
            printf("\\");
        } else if (isprint(c)) {
            printf("%c", buf.data[i]);
        } else if (c == '\x1b') {
            printf("\\e");
        } else {
            printf("\\x%02x", c);
        }
    }

    printf("\n");
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
        fake.cells[0].style.forecolor = BLUE;
        fake.cells[1].codes[0] = 'e';
        fake.cells[2].codes[0] = 'l';
        fake.cells[3].codes[0] = 'l';
        fake.cells[4].codes[0] = 'o';

        fake.cells[fake.cols+0].codes[0] = 'W';
        fake.cells[fake.cols+1].codes[0] = 'o';
        fake.cells[fake.cols+1].style.forecolor = RED;
        fake.cells[fake.cols+1].style.backcolor = GREEN;
        fake.cells[fake.cols+2].codes[0] = 'r';
        fake.cells[fake.cols+3].codes[0] = 'l';
        fake.cells[fake.cols+4].codes[0] = 'd';

        char exp[] = "\e[1;1H\e[34mh\e[2;1H\e[37mW\e[31;42mo";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);

        assertEqBuf("Adds colors", buf.data, buf.used, exp, strlen(exp));
    }

    buf_free(&buf);
    screen_free(&fake);
    screen_free(&real);

    return 0;
}
