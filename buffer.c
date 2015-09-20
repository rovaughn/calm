#include "buffer.h"
#include <string.h>

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

void buf_push(buffer_t *buf, void *data, size_t n) {
    void *dest = buf_reserve(buf, n);
    memcpy(dest, data, n);
}

void buf_push_str(buffer_t *buf, const char *str) {
    size_t n = strlen(str);
    void *dest = buf_reserve(buf, n);
    memcpy(dest, str, n);
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

