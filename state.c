#include "state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "field.h"

struct state {
  field_state_t field;
};

state_t *state_new(void) {
  state_t *s = malloc(sizeof *s);

  s->field = new_field_state();

  return s;
}

void state_render(state_t *state, screen_t *screen) {
  screen_clear(screen, ' ');
  field_state_render(&state->field, screen, 0, 0);
  screen->cells[10] = 0xa2;
}

void state_apply_event(state_t *state, event_t e) {
  field_state_apply(&state->field, e);
}

