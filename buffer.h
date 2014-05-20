#ifndef BUFFER_H
#define BUFFER_H
#include <stdint.h>
#include <unistd.h>

typedef struct {
  size_t   len;
  size_t   cap;
  uint8_t *buf;
} buffer_t;


buffer_t new_buffer(size_t len, size_t cap);
void free_buffer(buffer_t *b);
void buffer_append(buffer_t *b, uint8_t i);
void buffer_extend(buffer_t *b, uint8_t *data, size_t len);

#endif
