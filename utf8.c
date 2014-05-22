#include "utf8.h"
#include <stdio.h>
#include <string.h>

int pututf8(char *out, uint32_t c) {
  memset(out, 0, UTF8_LEN);

  if (c <= 0x7f) {
    out[0] = c;
    return 1;
  } else if (c <= 0x7ff) {
    out[0] = (0xc0 | ((c >>  6) & 0x1f));
    out[1] = (0x80 | ((c >>  0) & 0x3f));
    return 2;
  } else if (c <= 0xffff) {
    out[0] = (0xe0 | ((c >> 12) & 0x0f));
    out[1] = (0x80 | ((c >>  6) & 0x3f));
    out[2] = (0x80 | ((c >>  0) & 0x3f));
    return 3;
  } else if (c <= 0x1fffff) {
    out[0] = (0xf0 | ((c >> 18) & 0x07));
    out[1] = (0x80 | ((c >> 12) & 0x3f));
    out[2] = (0x80 | ((c >>  6) & 0x3f));
    out[3] = (0x80 | ((c >>  0) & 0x3f));
    return 4;
  } else if (c <= 0x3ffffff) {
    out[0] = (0xf8 | ((c >> 24) & 0x03));
    out[1] = (0x80 | ((c >> 18) & 0x3f));
    out[2] = (0x80 | ((c >> 12) & 0x3f));
    out[3] = (0x80 | ((c >>  6) & 0x3f));
    out[4] = (0x80 | ((c >>  0) & 0x3f));
    return 5;
  } else if (c <= 0x7fffffff) {
    out[0] = (0xfc | ((c >> 30) & 0x01));
    out[1] = (0x80 | ((c >> 24) & 0x3f));
    out[2] = (0x80 | ((c >> 18) & 0x3f));
    out[3] = (0x80 | ((c >> 12) & 0x3f));
    out[4] = (0x80 | ((c >>  6) & 0x3f));
    out[5] = (0x80 | ((c >>  0) & 0x3f));
    return 6;
  } else {
    return 0;
  }
}

