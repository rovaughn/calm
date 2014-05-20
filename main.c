#include <stdio.h>
#include <stdbool.h>
#include "screen.h"
#include "event.h"
#include "buffer.h"
#include "state.h"

int main(void) {
  screen_t screen,
           realScreen;

  state_t *state = state_new();

  init_screen(&screen, ' ');
  init_screen(&realScreen, '\0'); // Filled with nulls at first, to make sure it gets overwritten.

  prepare_events();

  while (true) {
    state_render(state, &screen);
    draw(&realScreen, &screen);

    event_t e = await_event();

    switch (e.type) {
    default:
      state_apply_event(state, &e);
    break;
    }
  }

  return 0;
}

