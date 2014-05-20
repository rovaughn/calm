#include "buffer.h"
#include <stdlib.h>
#include <string.h>

buffer_t new_buffer(size_t len, size_t cap) {
  uint8_t *buf = malloc(cap * sizeof *buf);

  buffer_t b = {
    .len = len,
    .cap = cap,
    .buf = buf
  };

  return b;
}

void free_buffer(buffer_t *b) {
  b->len = 0;
  b->cap = 0;

  free(b->buf);
}

void buffer_append(buffer_t *b, uint8_t i) {
  if (b->len == b->cap) {
    b->cap = 1 + 2*b->cap;
    b->buf = realloc(b->buf, b->cap * sizeof *b->buf);
  }

  b->buf[b->len] = i;
  b->len += 1;
}

void buffer_insert(buffer_t *b, uint8_t i, size_t pos) {
  if (b->len == b->cap) {
    b->cap = 1 + 2*b->cap;
    b->buf = realloc(b->buf, b->cap * sizeof *b->buf);
  }

  memmove(&b->buf[pos + 1], &b->buf[pos], b->len - pos);

  b->buf[pos] = i;
  b->len += 1;
}

void buffer_extend(buffer_t *b, uint8_t *data, size_t len) {
  size_t needLen = b->len + len,
         newCap  = b->cap;

  while (needLen < newCap) {
    newCap = 1 + 2*newCap;
  }

  if (newCap > b->cap) {
    b->cap = newCap;
    b->buf = realloc(b->buf, b->cap * sizeof *b->buf);
  }

  memcpy(&b->buf[b->len], data, len);
  b->len = needLen;
}

void buffer_delete(buffer_t *b, size_t pos) {
  if (pos < b->len) {
    memmove(&b->buf[pos], &b->buf[pos + 1], b->len - pos - 1);
    b->len -= 1;
  }
}

