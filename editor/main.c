#include "../screen.h"
#include "../event.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    buffer_t buf = buf_new(1);
    screen_t fake, real;

    int rows, cols;
    read_dimensions(&rows, &cols);

    fake_screen_init(&fake, rows, cols);
    real_screen_init(&buf, &real, rows, cols);
    prepare_events();

    int x = 0,
        y = 0;
    char text[cols*rows];
    bool marks[cols*rows];

    memset(text, ' ', sizeof text);
    memset(marks, 0, sizeof marks);

await: {
    event_t e = await_event();

    switch (e.type) {
    case E_SIG:
        goto quit;
    break;
    case E_KEY:
        switch (e.key) {
        case '\b':
            if (x > 0) x -= 1;
        break;
        case '\n': x  = 0; y += 1; break;
        case '~':
            marks[x + y*cols] = !marks[x + y*cols];
            x++;
        break;
        default:
            text[x + y*cols] = e.key;
            x++;
        break;
        }
    break;
    case E_UP: y -= 1; break;
    case E_DOWN: y += 1; break;
    case E_LEFT: x -= 1; break;
    case E_RIGHT: x += 1; break;
    default:
    break;
    }

    int cx, cy;
    for (cx = 0; cx < cols; cx++) {
        for (cy = 0; cy < rows; cy++) {
            fake.cells[cx + cy*cols].codes[0] = text[cx + cy*cols];
        }
    }

    int j;
    for (j = 0; j < cols*rows; j++) {
        if (marks[j]) {
            fake.cells[j].style.back.rgb = 0xff0000;
        }
    }

    fake.cursor.x = x;
    fake.cursor.y = y;
    fake.cursor.visible = true;

    screen_flush(&buf, &fake, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    buf.used = 0;
    fake_screen_reset(&fake, 25, 80);

    goto await;
}

quit:
    cleanup_events();
    real_screen_cleanup(&buf, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    
    screen_free(&fake);
    screen_free(&real);
    buf_free(&buf);

    return 0;
}
