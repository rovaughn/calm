#include "state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "field.h"
#include "dots.h"
#include <math.h>

struct state {
  double t,
         v;
};

state_t *state_new(void) {
  state_t *s = malloc(sizeof *s);

  s->t = 0;
  s->v = 1/60.0;

  return s;
}

void state_render(state_t *state, screen_t *screen) {
  screen_clear(screen, ' ');

  {
    char out[100];
    snprintf(out, sizeof out, "Velocity: %f", state->v);

    size_t i;
    for (i = 0; i < strlen(out); ++i) {
      screen->cells[i] = out[i];
    }
  }

  {
    int cols = 2*screen->cols,
        rows = 4*(screen->rows - 1);
    bool dots[cols * rows];

    memset(dots, 0, sizeof dots);

    double x = 0;
    for (x = 0; x < cols; ++x) {
      double y = (rows/2)+(rows/4)*sin(state->t + x/10.0);

      dots[((int)y)*cols + (int)x] = 1;
    }

    dots_render(screen, dots, cols, rows, 0, 1);
  }

  screen->showcursor = false;
}

bool state_apply_event(state_t *state, event_t e) {
  switch (e.type) {
  case E_UP:
    state->v *= 2.0;
  break;
  case E_DOWN:
    state->v /= 2.0;
  break;
  default:
  break;
  }

  state->t += state->v;
  return true;
}

