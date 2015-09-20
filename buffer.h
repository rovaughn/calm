#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    size_t   capacity,
             used;
    uint8_t *data;
} buffer_t;

buffer_t buf_new(size_t capacity);
void buf_free(buffer_t *buf);

void buf_push(buffer_t *buf, void *data, size_t n);
void buf_push_str(buffer_t *buf, const char *str);
void buf_push_int(buffer_t *buf, unsigned i);

#endif
