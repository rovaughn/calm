#ifndef ASSERT_H
#define ASSERT_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../error.h"

#define RED   "\x1b[31m"
#define GREEN "\x1b[32m"
#define CLEAR "\x1b[0m"

#define SUCCESS "\xe2\x9c\x94"
#define FAILURE "\xe2\x9c\x98"

void assert(const char *message, bool condition) {
  if (condition) {
    fprintf(stderr, GREEN " " SUCCESS " %s\n" CLEAR, message);
  } else {
    fprintf(stderr, RED " " FAILURE " %s\n" CLEAR, message);
    exit(1);
  }
}

void assertError(const char *message, const char *err) {
  if (err) {
    fprintf(stderr, RED " " FAILURE " %s: %s\n" CLEAR, message, err);
    exit(1);
  } else {
    fprintf(stderr, GREEN " " SUCCESS " %s\n" CLEAR, message);
  }
}

#define assertEq(m,a,e) assertEq_((m),(intptr_t)(a),(intptr_t)(e))
void assertEq_(const char *message, intptr_t actual, intptr_t expected) {
  if (expected == actual) {
    fprintf(stderr, GREEN " " SUCCESS " %s\n" CLEAR, message);
  } else {
    fprintf(stderr, RED " " FAILURE " %s: Expected %lu, got %lu\n" CLEAR, message, (uint64_t)expected, (uint64_t)actual);
    exit(1);
  }
}

void assertEqBuf(const char *message, void *actual, size_t alen, void *expected, size_t elen) {
  if (elen != alen) {
    fprintf(stderr, RED " " FAILURE " %s: Expected buf of size %lu, but is %lu\n", message, elen, alen);
    exit(1);
  } else if (memcmp(actual, expected, alen) != 0) {
    size_t i;

    fprintf(stderr, RED " " FAILURE " %s: Expected ", message);
    for (i = 0; i < elen; ++i) fprintf(stderr, "%02x", ((uint8_t*)expected)[i]);
    fprintf(stderr, ", got ");
    for (i = 0; i < alen; ++i) fprintf(stderr, "%02x", ((uint8_t*)actual)[i]);
    fprintf(stderr, "\n");
    exit(1);
  } else {
    fprintf(stderr, GREEN " " SUCCESS " %s\n" CLEAR, message);
  }
}

#endif
