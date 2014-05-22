// ../buffer.o
#include "../buffer.h"
#include "assert.h"

int main(void) {
  {
    buffer_t b = new_buffer(1, 2);

    assertEq("Starts with correct len", b.len, 1);
    assertEq("Starts with correct cap", b.cap, 2);
    assertEq("Initializes portion of buffer", b.buf[0], 0);

    buffer_append(&b, 'a');
    buffer_append(&b, 'c');
    buffer_insert(&b, 'b', 2);
    buffer_delete(&b, 0);

    uint32_t expected[] = {'a', 'b', 'c'};

    assertEqBuf("Append, insert, delete work", b.buf, b.len*sizeof *b.buf, expected, sizeof expected);
    assert("The size of the buffer grew sufficiently", b.cap >= b.len);

    free_buffer(&b);
  }

  return 0;
}

