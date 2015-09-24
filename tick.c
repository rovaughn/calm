#include "tick.h"
#include <time.h>

void init_tick_ms(tick_t *tick, unsigned ms) {
    struct timespec ts_now;

    clock_gettime(CLOCK_MONOTONIC, &ts_now);
    tick->start_ns    = ts_now.tv_sec*1000000000 + ts_now.tv_nsec;
    tick->interval_ns = ms*1000000;
}

bool ticked(tick_t *tick) {
    struct timespec ts_now;

    clock_gettime(CLOCK_MONOTONIC, &ts_now);

    uint64_t now  = ts_now.tv_sec*1000000000 + ts_now.tv_nsec,
             next = tick->start_ns + tick->interval_ns;

    if (next < now) {
        tick->start_ns = next;
        return true;
    } else {
        return false;
    }
}

