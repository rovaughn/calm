#include "buffer.h"
#include <string.h>
#include "utf8.h"

buffer_t buf_new(size_t capacity) {
    buffer_t b;

    b.data = malloc(capacity);
    b.used = 0;
    b.capacity = capacity;

    return b;
}

void buf_free(buffer_t *buf) {
    free(buf->data);
}

void *buf_reserve(buffer_t *buf, size_t n) {
    size_t old_used     = buf->used,
           new_used     = buf->used + n,
           new_capacity = buf->capacity;

    while (new_used > new_capacity) {
        new_capacity *= 2;
    }

    buf->data     = realloc(buf->data, new_capacity);
    buf->used     = new_used;
    buf->capacity = new_capacity;

    return &buf->data[old_used];
}

void buf_push(buffer_t *buf, const void *data, size_t n) {
    memcpy(buf_reserve(buf, n), data, n);
}

void buf_push_str(buffer_t *buf, const char *str) {
    buf_push(buf, str, strlen(str));
}

void buf_push_utf8(buffer_t *buf, const wchar_t *codes, size_t ncodes) {
    size_t  reserved = ncodes*UTF8_LEN;
    void   *dest     = buf_reserve(buf, reserved);
    size_t  used     = putsutf8(dest, codes, ncodes);

    buf->used -= reserved - used;
}

static unsigned count_digits(unsigned n) {
    unsigned digits = 0;

    while (n > 0) {
        n /= 10;
        digits++;
    }

    return digits;
}

void buf_push_int(buffer_t *buf, unsigned n) {
    if (n == 0) {
        buf_push_str(buf, "0");
        return;
    }

    unsigned digits = count_digits(n);
    unsigned i = digits - 1;
    uint8_t *dest = buf_reserve(buf, digits);

    while (n > 0) {
        unsigned digit = n % 10;
        n /= 10;
        dest[i] = '0' + digit;
        i--;
    }
}

