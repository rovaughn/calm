#ifndef STATE_H
#define STATE_H

#include "event.h"
#include "screen.h"
#include "buffer.h"

typedef struct state state_t;

state_t *state_new(void);
void state_render(state_t *state, screen_t *screen);
void state_apply_event(state_t *state, event_t event);

#endif
