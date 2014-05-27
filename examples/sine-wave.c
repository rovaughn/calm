#include <stdio.h>
#include <stdbool.h>
#include "../screen.h"
#include "../event.h"
#include "../buffer.h"
#include "../state.h"
#include <time.h>

struct timespec event_sleep = { .tv_sec = 0, .tv_nsec = 16666666 }; // 1/60 second

int main(void) {
  screen_t screen,
           realScreen;

  state_t *state = state_new();

  init_screen(&screen, ' ');
  init_screen(&realScreen, '\0'); // Filled with nulls at first, to make sure
                                  // it gets overwritten.

  prepare_events();

  state_render(state, &screen);
  draw(&realScreen, &screen);

  while (true) {
    event_t e = await_event();
    
    if (state_apply_event(state, e)) {
      state_render(state, &screen);
      draw(&realScreen, &screen);
    }

    nanosleep(&event_sleep, NULL);
  }

  return 0;
}

