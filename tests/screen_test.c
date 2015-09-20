// ../screen.o ../buffer.o ../utf8.o ../wcwidth.o
#include "../screen.h"
#include "assert.h"
#include <ctype.h>

const char *checkCleared(screen_t *screen) {
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
            printf("\\x%02x ", c);
        }
    }

    printf("\n");
}

int main(void) {
    buffer_t buf = buf_new(1);
    screen_t screen1, screen2;
    screen_t *fake = &screen1,
             *real = &screen2;

    {
        char exp[] = "\e[H\e[2J";

        buf.used = 0;
        screen_init(&buf, fake, real, 25, 80);

        assertEqBuf("Init clears screen", buf.data, buf.used, exp, strlen(exp));

        assertEq("fake->rows", fake->rows, 25);
        assertEq("fake->cols", fake->cols, 80);
        assertEq("real->rows", real->rows, 25);
        assertEq("real->cols", real->cols, 80);
        
        assertEq("fake->cursorx", fake->cursorx, 0);
        assertEq("fake->cursory", fake->cursory, 0);
        assertEq("fake->showcursor", fake->showcursor, false);

        assertError("fake is cleared", checkCleared(fake));
        assertError("real is cleared", checkCleared(real));
    }

    {
        char exp[] = "";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);
        assertEqBuf("Nothing", buf.data, buf.used, exp, strlen(exp));
    }

    {
        fake->cells[0].codes[0] = 'h';
        fake->cells[1].codes[0] = 'e';
        fake->cells[2].codes[0] = 'l';
        fake->cells[3].codes[0] = 'l';
        fake->cells[4].codes[0] = 'o';

        fake->cells[fake->cols+0].codes[0] = 'w';
        fake->cells[fake->cols+1].codes[0] = 'o';
        fake->cells[fake->cols+2].codes[0] = 'r';
        fake->cells[fake->cols+3].codes[0] = 'l';
        fake->cells[fake->cols+4].codes[0] = 'd';

        char exp[] = "hello\e[2;1Hworld";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);

        assertEqBuf("Writes multiple lines", buf.data, buf.used, exp, strlen(exp));
    }

    {
        fake->cells[0].codes[0] = 'h';
        fake->cells[0].style.forecolor = BLUE;
        fake->cells[1].codes[0] = 'e';
        fake->cells[2].codes[0] = 'l';
        fake->cells[3].codes[0] = 'l';
        fake->cells[4].codes[0] = 'o';

        fake->cells[fake->cols+0].codes[0] = 'w';
        fake->cells[fake->cols+1].codes[0] = 'o';
        fake->cells[fake->cols+1].style.forecolor = RED;
        fake->cells[fake->cols+1].style.backcolor = GREEN;
        fake->cells[fake->cols+2].codes[0] = 'r';
        fake->cells[fake->cols+3].codes[0] = 'l';
        fake->cells[fake->cols+4].codes[0] = 'd';

        // TODO: It'd be better if this were "\e[34m\e[2;2H\e[31;47m"
        char exp[] = "\e[34m\e[2;2H\e[31m\e[41m";

        buf.used = 0;
        screen_flush(&buf, &fake, &real);
        show(buf);

        assertEqBuf("Adds colors", buf.data, buf.used, exp, strlen(exp));
    }

    buf_free(&buf);
    screen_free(&screen1);
    screen_free(&screen2);

    return 0;
}
