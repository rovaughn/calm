#include "state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct state {
  buffer_t pendingCommand;
  int      pendingCommandCursor;
};

state_t *state_new(void) {
  state_t *s = malloc(sizeof *s);

  s->pendingCommand = new_buffer(0, 32);
  s->pendingCommandCursor = 0;

  return s;
}

void state_render(state_t *state, screen_t *screen) {
  memset(screen->cells, ' ', screen->cols * screen->rows * sizeof *screen->cells);
  screen->cursorx = state->pendingCommandCursor;
  memcpy(screen->cells, state->pendingCommand.buf, state->pendingCommand.len);
}

void state_apply_event(state_t *state, event_t *e) {
  switch (e->type) {
  case E_KEY:
    buffer_insert(&state->pendingCommand, e->key, state->pendingCommandCursor);
    state->pendingCommandCursor += 1;
  break;
  case E_BACKSPACE:
    if (state->pendingCommandCursor > 0) {
      buffer_delete(&state->pendingCommand, state->pendingCommandCursor - 1);
      state->pendingCommandCursor -= 1;
    }
  break;
  case E_LEFT:
    if (state->pendingCommandCursor > 0) {
      state->pendingCommandCursor -= 1;
    }
  break;
  case E_RIGHT:
    if (state->pendingCommandCursor < state->pendingCommand.len) {
      state->pendingCommandCursor += 1;
    }
  case E_HOME:
    state->pendingCommandCursor = 0;
  break;
  case E_END:
    state->pendingCommandCursor = state->pendingCommand.len;
  break;
  }
}

