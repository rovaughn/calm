#ifndef FIELD_H
#define FIELD_H
#include "buffer.h"
#include "event.h"
#include "screen.h"

typedef struct {
  buffer_t contents;
  size_t   cursor;
} field_state_t;

field_state_t new_field_state(void);
void field_state_apply(field_state_t *fs, event_t e);
void field_state_render(const field_state_t *fs, screen_t *screen, int x, int y);

#endif
