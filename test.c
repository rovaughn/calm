#include "screen.h"

int main(void) {
  screen_t screen1, screen2;
  screen_t *fake = &screen1,
           *real = &screen2;

  screen_init(fake, real);

  fake->cells[fake->cols + 0].codes[0] = 'a';
  fake->cells[fake->cols + 1].codes[0] = 'b';
  fake->cells[fake->cols + 2].codes[0] = 0x4e2d;
  fake->cells[fake->cols + 4].codes[0] = 'c';
  fake->cells[fake->cols + 4].codes[1] = 0x300;
  fake->cells[fake->cols + 5].codes[0] = 'd';
  fake->cells[fake->cols + 5].style    = RED;

  screen_flush(&fake, &real);

  return 0;
}

