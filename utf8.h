#ifndef UTF8_H
#define UTF8_H
#include <stdint.h>
#include <wchar.h>

#define UTF8_LEN 6

int pututf8(char *out, wchar_t c);
size_t putsutf8(char *out, wchar_t *wcs, size_t len);

#endif
