#include "../screen.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(void) {
    buffer_t buf = buf_new(1);
    screen_t fake, real;

    int rows, cols;
    read_dimensions(&rows, &cols);

    fake_screen_init(&fake, rows, cols);
    real_screen_init(&buf, &real, rows, cols);

    struct timespec req = {.tv_sec = 0, .tv_nsec = 100000000};
    struct timespec rem;

    int i = 0;
    while (i < 50) {
        int start = rows*cols/2;
        fake.cells[start+0].codes[0] = 'H';
        fake.cells[start+1].codes[0] = 'E';
        fake.cells[start+2].codes[0] = 'L';
        fake.cells[start+3].codes[0] = 'L';
        fake.cells[start+4].codes[0] = 'O';
        fake.cells[start+5].codes[0] = '!';
        fake.cells[cols+start+0].codes[0] = 'w';
        fake.cells[cols+start+1].codes[0] = 'o';
        fake.cells[cols+start+2].codes[0] = 'r';
        fake.cells[cols+start+3].codes[0] = 'l';
        fake.cells[cols+start+4].codes[0] = 'd';
        fake.cells[cols+start+0].style.back.rgb = 0x000000;
        fake.cells[cols+start+1].style.back.rgb = 0x101010;
        fake.cells[cols+start+2].style.back.rgb = 0x202020;
        fake.cells[cols+start+3].style.back.rgb = 0x303030;
        fake.cells[cols+start+4].style.back.rgb = 0x404040;

        fake.cells[start + (i + 0) % 5].style.fore.rgb = 0xff0000;
        fake.cells[start + (i + 0) % 5].style.back.rgb = 0x00ffff;
        fake.cells[start + (i + 1) % 5].style.fore.rgb = 0xffff00;
        fake.cells[start + (i + 1) % 5].style.back.rgb = 0x0000ff;
        fake.cells[start + (i + 2) % 5].style.fore.rgb = 0x00ff00;
        fake.cells[start + (i + 2) % 5].style.back.rgb = 0xff00ff;
        fake.cells[start + (i + 3) % 5].style.fore.rgb = 0x00ffff;
        fake.cells[start + (i + 3) % 5].style.back.rgb = 0xff0000;
        fake.cells[start + (i + 4) % 5].style.fore.rgb = 0x0000ff;
        fake.cells[start + (i + 4) % 5].style.back.rgb = 0xffff00;
        i++;

        screen_flush(&buf, &fake, &real);
        write(STDOUT_FILENO, buf.data, buf.used);
        buf.used = 0;
        fake_screen_reset(&fake, 25, 80);

        nanosleep(&req, &rem);
    }

    real_screen_cleanup(&buf, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    
    screen_free(&fake);
    screen_free(&real);
    buf_free(&buf);
}
