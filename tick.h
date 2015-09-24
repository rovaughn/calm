#ifndef TICK_H
#define TICK_H
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t start_ns,
             interval_ns;
} tick_t;

void init_tick_ms(tick_t *tick, unsigned ms);
bool ticked(tick_t *tick);

#endif
