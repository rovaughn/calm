#ifndef READKEY_H
#define READKEY_H
#include <stdint.h>

enum {
    KEY_AGAIN = -1, // read_key can be retried again immediately
    KEY_BLOCK = -2, // no input
    KEY_DEL = 127,
    KEY_END, KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT, KEY_HOME
};

typedef int16_t key_t;

void init_readkey(void);
key_t readkey(void);
void cleanup_readkey(void);

#endif
