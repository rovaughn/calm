#include "dots.h"

void dots_render(screen_t *screen, bool *dots, int cols, int rows, int x, int y) {
  int screencols = cols / 2 + cols % 2,
      screenrows = rows / 4 + (rows % 4 ? 1 : 0);

  int xs, ys;
  for (ys = 0; ys < screenrows; ++ys) {
    for (xs = 0; xs < screencols; ++xs) {
      int yb = ys*4,
          xb = xs*2;

      uint32_t c = 0x2800;

      c |= dots[(yb+0)*cols + (xb+0)] << 0;
      c |= dots[(yb+0)*cols + (xb+1)] << 3;
      c |= dots[(yb+1)*cols + (xb+0)] << 1;
      c |= dots[(yb+1)*cols + (xb+1)] << 4;
      c |= dots[(yb+2)*cols + (xb+0)] << 2;
      c |= dots[(yb+2)*cols + (xb+1)] << 5;
      c |= dots[(yb+3)*cols + (xb+0)] << 6;
      c |= dots[(yb+3)*cols + (xb+1)] << 7;

      screen_put(screen, xs + x, ys + y, c);
    }
  }
}

