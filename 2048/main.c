#include "../screen.h"
#include "../readkey.h"
#include "../tick.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

static int caught_signal = 0;

static void handle_signal(int signum) {
  caught_signal = signum;
}

// Picks from uniformly distributed integers of [a, b]
int randint(int a, int b) {
    int range      = b - a + 1,
        usable_max = RAND_MAX - (RAND_MAX % range);

    while (1) {
        int r = rand();

        if (r <= usable_max) {
            return (r % range) + a;
        }
    }
}
typedef struct {
    int x, y;
} point_t;

#define WIDTH 4

typedef struct {
    enum {
        PLAYING, WIN, LOSE
    } state;

    unsigned tiles[WIDTH*WIDTH];
} state_t;


void init_state(state_t *state) {
    state->state = PLAYING;
    memset(&state->tiles, 0, sizeof state->tiles);
}

const char *number_names[12] = { "    ", "   2", "   4", "   8", "  16", "  32",
                                 "  64", " 128", " 256", " 512", "1024", "2048"};
unsigned colors[12] = {0x000000, 0xeee4da, 0xedec0c8, 0xf2b179, 0xf59563, 0xf67c5f, 0xf65e3b, 0xedcf72, 0xedcc61, 0xedc850, 0xedc53f, 0xedc22e};

void render(const state_t *state, screen_t *screen) {
    if (state->state == LOSE) {
        screen->cells[0].codes[0] = 'L';
        return;
    }

    int x, y, j;
    for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < WIDTH; y++) {
    for (j = 0; j < 4; j++) {
        unsigned    tile = state->tiles[x + WIDTH*y];
        const char *name = number_names[tile];

        screen->cells[5*x + j + screen->cols*y].codes[0] = name[j];
        screen->cells[5*x + j + screen->cols*y].style.fore.rgb = colors[tile];
    }
    }
    }
}

bool random_tile(state_t *state) {
    unsigned x, y;

    for (x = 0; x < WIDTH; x++)
    for (y = 0; y < WIDTH; y++) {
        if (state->tiles[x + y*WIDTH] == 0) {
            goto choose;
        }
    }

    return false;

choose:
    while (true) {
        unsigned x = randint(0, WIDTH - 1),
                 y = randint(0, WIDTH - 1);

        if (state->tiles[x + y*WIDTH] == 0) {
            state->tiles[x + y*WIDTH] = 1;
            return true;
        }
    }
}

typedef enum { NORTH, SOUTH, EAST, WEST } direction_t;

void move(state_t *state, direction_t direction) {
    int x, y;
    bool changed;

    do {
        changed = false;
        switch (direction) {
        case NORTH:
            for (x = 0; x < WIDTH; x++) {
            for (y = 0; y < WIDTH - 1; y++) {
                unsigned *here = &state->tiles[x + y*WIDTH],
                         *next = &state->tiles[x + (y + 1)*WIDTH];

                if (*here == 0 && *next > 0) {
                    *here = *next;
                    *next = 0;
                    changed = true;
                } else if (*here > 0 && *here == *next) {
                    *here += 1;
                    *next = 0;
                    changed = true;
                }
            }
            }
            break;
        case SOUTH:
            for (x = 0; x < WIDTH; x++) {
            for (y = WIDTH - 1; y > 0; y--) {
                unsigned *here = &state->tiles[x + y*WIDTH],
                         *next = &state->tiles[x + (y - 1)*WIDTH];

                if (*here == 0 && *next > 0) {
                    *here = *next;
                    *next = 0;
                    changed = true;
                } else if (*here > 0 && *here == *next) {
                    *here += 1;
                    *next = 0;
                    changed = true;
                }
            }
            }
            break;
        case WEST:
            for (y = 0; y < WIDTH; y++) {
            for (x = 0; x < WIDTH - 1; x++) {
                unsigned *here = &state->tiles[x + y*WIDTH],
                         *next = &state->tiles[(x + 1) + y*WIDTH];

                if (*here == 0 && *next > 0) {
                    *here = *next;
                    *next = 0;
                    changed = true;
                } else if (*here > 0 && *here == *next) {
                    *here += 1;
                    *next = 0;
                    changed = true;
                }
            }
            }
            break;
        case EAST:
            for (y = 0; y < WIDTH; y++) {
            for (x = WIDTH - 1; x > 0; x--) {
                unsigned *here = &state->tiles[x + y*WIDTH],
                         *next = &state->tiles[(x - 1) + y*WIDTH];

                if (*here == 0 && *next > 0) {
                    *here = *next;
                    *next = 0;
                    changed = true;
                } else if (*here > 0 && *here == *next) {
                    *here += 1;
                    *next = 0;
                    changed = true;
                }
            }
            }
            break;
        }
    } while (changed);

    if (!random_tile(state)) {
        state->state = LOSE;
    }
}

int main(void) {
    srand(time(NULL));

    buffer_t buf = buf_new(1);
    screen_t fake, real;

    int rows, cols;
    read_dimensions(&rows, &cols);

    fake_screen_init(&fake, rows, cols);
    real_screen_init(&buf, &real, rows, cols);
    init_readkey();

    struct timespec req = {.tv_nsec = 1000000000/60};

    state_t state;
    init_state(&state);

    random_tile(&state);

    signal(SIGINT, handle_signal);

    bool dirty = true;

    while (true) {
        if (caught_signal != 0) {
            goto quit;
        }

        key_t k = readkey();

        switch (k) {
        case KEY_UP:    move(&state, NORTH); dirty = true; break;
        case KEY_DOWN:  move(&state, SOUTH); dirty = true; break;
        case KEY_RIGHT: move(&state, EAST);  dirty = true; break;
        case KEY_LEFT:  move(&state, WEST);  dirty = true; break;
        }

        if (dirty) {
            render(&state, &fake);

            screen_flush(&buf, &fake, &real);
            write(STDOUT_FILENO, buf.data, buf.used);
            buf.used = 0;
            fake_screen_reset(&fake, 25, 80);
            dirty = false;
        }

        if (state.state == WIN || state.state == LOSE) {
            goto quit;
        }

        nanosleep(&req, NULL);
    }

quit:
    cleanup_readkey();
    real_screen_cleanup(&buf, &real);
    write(STDOUT_FILENO, buf.data, buf.used);
    
    screen_free(&fake);
    screen_free(&real);
    buf_free(&buf);

    return 0;
}
