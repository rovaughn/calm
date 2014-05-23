#ifndef DOTS_H
#define DOTS_H
#include <stdbool.h>
#include "screen.h"

void dots_render(screen_t *screen, bool *dots, int cols, int rows, int x, int y);

#endif
