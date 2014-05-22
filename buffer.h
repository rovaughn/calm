#ifndef BUFFER_H
#define BUFFER_H
#include <stdint.h>
#include <unistd.h>

typedef struct {
  size_t    len;
  size_t    cap;
  uint32_t *buf;
} buffer_t;


buffer_t new_buffer(size_t len, size_t cap);
void free_buffer(buffer_t *b);
void buffer_append(buffer_t *b, uint32_t i);
void buffer_extend(buffer_t *b, uint32_t *data, size_t len);
void buffer_insert(buffer_t *b, uint32_t i, size_t pos);
void buffer_delete(buffer_t *b, size_t pos);

#endif
