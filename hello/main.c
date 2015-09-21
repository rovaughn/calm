#include "../screen.h"
#include <unistd.h>
#include <stdio.h>

int main(void) {
    buffer_t buf = buf_new(1);
    screen_t fake, real;

    fake_screen_init(&fake, 25, 80);
    real_screen_init(&buf, &real, 25, 80);

    int i = 0;
    while (i < 10) {
        fake.cells[0].codes[0] = 'h';
        fake.cells[1].codes[0] = 'e';
        fake.cells[2].codes[0] = 'l';
        fake.cells[3].codes[0] = 'l';
        fake.cells[4].codes[0] = 'o';

        fake.cells[i++ % 5].style.forecolor = RED;

        screen_flush(&buf, &fake, &real);
        write(STDOUT_FILENO, buf.data, buf.used);
        buf.used = 0;
        fake_screen_reset(&fake, 25, 80);
        sleep(1);
    }

    real_screen_cleanup(&buf, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    
    screen_free(&fake);
    screen_free(&real);
    buf_free(&buf);
}
