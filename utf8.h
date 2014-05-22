#ifndef UTF8_H
#define UTF8_H
#include <stdint.h>

#define UTF8_LEN 6

int pututf8(char *out, uint32_t c);

#endif
