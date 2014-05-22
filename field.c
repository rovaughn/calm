#include "field.h"
#include <string.h>

field_state_t new_field_state(void) {
  field_state_t fs = {
    .contents = new_buffer(0, 32),
    .cursor   = 0
  };

  return fs;
}

void field_state_apply(field_state_t *fs, event_t e) {
  switch (e.type) {
  case E_KEY:
    buffer_insert(&fs->contents, e.key, fs->cursor);
    fs->cursor += 1;
  break;
  case E_BACKSPACE:
    if (fs->cursor > 0) {
      buffer_delete(&fs->contents, fs->cursor - 1);
      fs->cursor -= 1;
    }
  break;
  case E_LEFT:
    if (fs->cursor > 0) {
      fs->cursor -= 1;
    }
  break;
  case E_RIGHT:
    if (fs->cursor < fs->contents.len) {
      fs->cursor += 1;
    }
  case E_HOME:
    fs->cursor = 0;
  break;
  case E_END:
    fs->cursor = fs->contents.len;
  break;
  }
}

void field_state_render(const field_state_t *fs, screen_t *screen, int x, int y) {
  screen->cursorx = x + fs->cursor;
  screen->cursory = y;

  memcpy(screen->cells, fs->contents.buf, fs->contents.len);
}

