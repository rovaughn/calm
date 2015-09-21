#include "../screen.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int main(void) {
    buffer_t buf = buf_new(1);
    screen_t fake, real;

    int rows = 20, cols = 20;
    //read_dimensions(&rows, &cols);

    fake_screen_init(&fake, rows, cols);
    real_screen_init(&buf, &real, rows, cols);

    struct timespec req = {.tv_sec = 0, .tv_nsec = 100000000};
    struct timespec rem;

    int x, y;

    double cx = rows / 2.0,
           cy = cols / 2.0;

    double maxradius = sqrt(pow(cx, 2) + pow(cy, 2));

    int t = 0;
    while (t < 50) {
        for (x = 0; x < cols; x++) {
            for (y = 0; y < rows; y++) {
                int i = x + y*cols;
                int shade = 128 + 128*sin(t + sqrt(pow(x - cx, 2) + pow(y - cy, 2)));

                if (shade > 255) {
                    shade = 255;
                }

                fake.cells[i].style.back.r = shade;
                fake.cells[i].style.back.g = shade;
                fake.cells[i].style.back.b = shade;
            }
        }

        screen_flush(&buf, &fake, &real);
        write(STDOUT_FILENO, buf.data, buf.used);
        buf.used = 0;
        fake_screen_reset(&fake, rows, cols);
        
        t++;
        nanosleep(&req, &rem);
    }

    real_screen_cleanup(&buf, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    
    screen_free(&fake);
    screen_free(&real);
    buf_free(&buf);
}
