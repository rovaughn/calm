// ../utf8.o
#include "../utf8.h"
#include "assert.h"

int main(void) {
  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0x24, 0};

    assertEq("\x24 len", pututf8(out, 0x0024), 1);
    assertEqBuf("\x24", out, 1, exp, 1);
  }

  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0xc2, 0xa2, 0};

    assertEq("\xc2\xa2 len", pututf8(out, 0x00a2), 2);
    assertEqBuf("\xc2\xa2", out, 2, exp, 2);
  }
  
  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0xe2, 0x82, 0xac, 0};

    assertEq("\xe2\x82\xac len", pututf8(out, 0x20ac), 3);
    assertEqBuf("\xe2\x82\xac", out, 3, exp, 3);
  }
 
  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0xe2, 0xa0, 0x93, 0};

    assertEq("\xe2\xa0\x93 len", pututf8(out, 0x2813), 3);
    assertEqBuf("\xe2\xa0\x93", out, 3, exp, 3);
  }

  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0xe4, 0xb8, 0xad, 0};

    assertEq("\xe4\xb8\xad len", pututf8(out, 0x4e2d), 3);
    assertEqBuf("\xe4\xb8\xad", out, 3, exp, 3);
  }

  {
    char out[UTF8_LEN],
         exp[UTF8_LEN] = {0xf0, 0xa4, 0xad, 0xa2, 0};

    assertEq("\xf0\xa4\xad\xa2 len", pututf8(out, 0x24b62), 4);
    assertEqBuf("\xf0\xa4\xad\xa2", out, 4, exp, 4);
  }

  return 0;
}

