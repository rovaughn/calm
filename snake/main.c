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

#define MAX_SEGMENTS 30

typedef struct {
    enum {
        PLAYING, WIN, LOSE
    } state;

    enum {
        NORTH, SOUTH, EAST, WEST
    } direction;

    time_t  start, now;
    point_t segments[MAX_SEGMENTS];
    int     nsegments;
    point_t food;
    int     rows, cols;
} state_t;

point_t point(int x, int y) {
    point_t p = {.x = x, .y = y};
    return p;
}

bool point_eq(point_t a, point_t b) {
    return a.x == b.x && a.y == b.y;
}

void init_state(state_t *state, int rows, int cols) {
    state->state = PLAYING;
    state->direction = SOUTH;
    state->segments[0] = point(5, 5);
    state->nsegments = 1;
    state->food = point(2, 2);
    state->rows = rows;
    state->cols = cols;
    state->start = 0;
    state->now = 0;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void render(const state_t *state, screen_t *screen) {
    int j;
    for (j = 1; j < state->cols + 1; j++) {
        screen->cells[j].codes[0] = '-';
        screen->cells[j + (state->rows+1)*screen->cols].codes[0] = '-';
    }
    for (j = 1; j < state->rows + 1; j++) {
        screen->cells[j*screen->cols].codes[0] = '|';
        screen->cells[state->cols + 1 + j*screen->cols].codes[0] = '|';
    }

    char elapsed_str[50],
         progress_str[50];

    unsigned elapsed = state->now - state->start;

    sprintf(elapsed_str, "%d:%02d elapsed", elapsed/60, elapsed%60);
    sprintf(progress_str, "%d/%d foods gathered", state->nsegments, MAX_SEGMENTS);

    for (j = 0; elapsed_str[j] != '\0'; j++) {
        screen->cells[j + (state->rows+2)*screen->cols].codes[0] = elapsed_str[j];
    }

    for (j = 0; progress_str[j] != '\0'; j++) {
        screen->cells[j + (state->rows+3)*screen->cols].codes[0] = progress_str[j];
    }

    if (state->state == LOSE) {
        int mid = state->cols/2 - 4 + screen->cols*state->rows/2;

        screen->cells[mid+0].codes[0] = 'Y';
        screen->cells[mid+1].codes[0] = 'o';
        screen->cells[mid+2].codes[0] = 'u';
        screen->cells[mid+3].codes[0] = ' ';
        screen->cells[mid+4].codes[0] = 'l';
        screen->cells[mid+5].codes[0] = 'o';
        screen->cells[mid+6].codes[0] = 's';
        screen->cells[mid+7].codes[0] = 'e';
        screen->cells[mid+8].codes[0] = '!';
    } else if (state->state == WIN) {
        int mid = state->cols/2 - 4 + screen->cols*state->rows/2;

        screen->cells[mid+0].codes[0] = 'Y';
        screen->cells[mid+1].codes[0] = 'o';
        screen->cells[mid+2].codes[0] = 'u';
        screen->cells[mid+3].codes[0] = ' ';
        screen->cells[mid+4].codes[0] = 'w';
        screen->cells[mid+5].codes[0] = 'i';
        screen->cells[mid+6].codes[0] = 'n';
        screen->cells[mid+7].codes[0] = '!';
    } else {
        for (j = 0; j < state->nsegments; j++) {
            point_t segment = state->segments[j];

            screen->cells[(segment.x+1) + (segment.y+1)*screen->cols].style.back.g = MAX(255 - 10*j, 128);
        }

        screen->cells[(state->food.x+1) + (state->food.y+1)*screen->cols].style.back.rgb = 0xff0000;
    }
}

void move(state_t *state) {
    point_t head = state->segments[0];
    point_t new_head;

process_direction:
    switch (state->direction) {
    case NORTH:
        if (head.y == 0) {
            if (head.x == state->cols - 1) {
                state->direction = WEST;
            } else if (head.x == 0) {
                state->direction = EAST;
            } else {
                state->direction = randint(0, 1) ? EAST : WEST;
            }
            goto process_direction;
        }

        new_head.x = head.x;
        new_head.y = head.y - 1;
        break;
    case SOUTH:
        if (head.y == state->rows - 1) {
            if (head.x == state->cols - 1) {
                state->direction = WEST;
            } else if (head.x == 0) {
                state->direction = EAST;
            } else {
                state->direction = randint(0, 1) ? EAST : WEST;
            }
            goto process_direction;
        }
        new_head.x = head.x;
        new_head.y = head.y + 1;
        break;
    case EAST:
        if (head.x == state->cols - 1) {
            if (head.y == state->rows - 1) {
                state->direction = NORTH;
            } else if (head.y == 0) {
                state->direction = SOUTH;
            } else {
                state->direction = randint(0, 1) ? NORTH : SOUTH;
            }
            goto process_direction;
        }
        new_head.x = head.x + 1;
        new_head.y = head.y;
        break;
    case WEST:
        if (head.x == 0) {
            if (head.y == state->rows - 1) {
                state->direction = NORTH;
            } else if (head.y == 0) {
                state->direction = SOUTH;
            } else {
                state->direction = randint(0, 1) ? NORTH : SOUTH;
            }
            goto process_direction;
        }
        new_head.x = head.x - 1;
        new_head.y = head.y;
        break;
    }

    int j;
    for (j = 0; j < state->nsegments; j++) {
        if (point_eq(new_head, state->segments[j])) {
            state->state = LOSE;
            return;
        }
    }

    if (point_eq(new_head, state->food)) {
        if (state->nsegments == MAX_SEGMENTS) {
            state->state = WIN;
            return;
        }

        memmove(&state->segments[1], state->segments, state->nsegments * sizeof *state->segments);
        state->nsegments++;

        state->food = point(randint(0, state->cols-1), randint(0, state->rows - 1));
    } else {
        memmove(&state->segments[1], state->segments, (state->nsegments - 1) * sizeof *state->segments);
    }

    state->segments[0] = new_head;
}

const char *direction_name(int d) {
    switch (d) {
    case NORTH: return "NORTH";
    case SOUTH: return "SOUTH";
    case EAST:  return "EAST";
    case WEST:  return "WEST";
    default:    return "?????";
    }
}

int main(void) {
    srand(time(NULL));

    buffer_t buf = buf_new(1);
    screen_t fake, real;

    int rows, cols;
    read_dimensions(&rows, &cols);

    int brows = 10, bcols = 10;

    fake_screen_init(&fake, rows, cols);
    real_screen_init(&buf, &real, rows, cols);
    init_readkey();

    struct timespec req = {.tv_nsec = 1000000000/60};

    state_t state;
    init_state(&state, brows, bcols);

    state.food = point(randint(0, state.cols - 1), randint(0, state.rows - 1));

    tick_t tick;
    init_tick_ms(&tick, 100);

    signal(SIGINT, handle_signal);

    bool dirty;

    struct timespec ts_now;

    clock_gettime(CLOCK_MONOTONIC, &ts_now);

    state.start = ts_now.tv_sec;

    while (true) {
        if (caught_signal != 0) {
            goto quit;
        }

        dirty = false;

        clock_gettime(CLOCK_MONOTONIC, &ts_now);

        if (state.now != ts_now.tv_sec) {
            state.now = ts_now.tv_sec;
            dirty = true;
        }

        key_t k = readkey();

        switch (k) {
        case KEY_UP:
            if (state.direction != SOUTH) {
                state.direction = NORTH;
                dirty = true;
            }
            break;
        case KEY_DOWN:
            if (state.direction != NORTH) {
                state.direction = SOUTH;
                dirty = true;
            }
            break;
        case KEY_RIGHT:
            if (state.direction != WEST) {
                state.direction = EAST;
                dirty = true;
            }
            break;
        case KEY_LEFT:
            if (state.direction != EAST) {
                state.direction = WEST;
                dirty = true;
            }
            break;
        }

        if (ticked(&tick)) {
            move(&state);
            dirty = true;
        }

        if (dirty) {
            render(&state, &fake);

            screen_flush(&buf, &fake, &real);
            write(STDOUT_FILENO, buf.data, buf.used);
            buf.used = 0;
            fake_screen_reset(&fake, 25, 80);
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
