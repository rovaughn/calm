#ifndef ASSERT_H
#define ASSERT_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../error.h"

#define _ASSERT_RED   "\x1b[31m"
#define _ASSERT_GREEN "\x1b[32m"
#define _ASSERT_CLEAR "\x1b[0m"

#define _ASSERT_SUCCESS "\xe2\x9c\x94"
#define _ASSERT_FAILURE "\xe2\x9c\x98"

void assert(const char *message, bool condition) {
  if (condition) {
    fprintf(stderr, _ASSERT_GREEN " " _ASSERT_SUCCESS " %s\n" _ASSERT_CLEAR, message);
  } else {
    fprintf(stderr, _ASSERT_RED " " _ASSERT_FAILURE " %s\n" _ASSERT_CLEAR, message);
    exit(1);
  }
}

void assertError(const char *message, const char *err) {
  if (err) {
    fprintf(stderr, _ASSERT_RED " " _ASSERT_FAILURE " %s: %s\n" _ASSERT_CLEAR, message, err);
    exit(1);
  } else {
    fprintf(stderr, _ASSERT_GREEN " " _ASSERT_SUCCESS " %s\n" _ASSERT_CLEAR, message);
  }
}

#define assertEq(m,a,e) assertEq_((m),(intptr_t)(a),(intptr_t)(e))
void assertEq_(const char *message, intptr_t actual, intptr_t expected) {
  if (expected == actual) {
    fprintf(stderr, _ASSERT_GREEN " " _ASSERT_SUCCESS " %s\n" _ASSERT_CLEAR, message);
  } else {
    fprintf(stderr, _ASSERT_RED " " _ASSERT_FAILURE " %s: Expected %lu, got %lu\n" _ASSERT_CLEAR, message, (uint64_t)expected, (uint64_t)actual);
    exit(1);
  }
}

void showbuf(void *buf, size_t len) {
    uint8_t *chars = buf;

    size_t i;
    for (i = 0; i < len; i++) {
        char c = chars[i];

        if (c == '\\') {
            fprintf(stderr, "\\");
        } else if (isprint(c)) {
            fprintf(stderr, "%c", c);
        } else if (c == '\x1b') {
            fprintf(stderr, "\\e");
        } else if (c == '\n') {
            fprintf(stderr, "\\n");
        } else {
            fprintf(stderr, "\\x%02x", c);
        }
    }
}

void assertEqBuf(const char *message, void *actual, size_t alen, void *expected, size_t elen) {
  if (elen != alen) {
    fprintf(stderr, _ASSERT_RED " " _ASSERT_FAILURE " %s: Expected buf of size %lu, but is %lu\n" _ASSERT_CLEAR, message, elen, alen);
    exit(1);
  } else if (memcmp(actual, expected, alen) != 0) {
    fprintf(stderr, _ASSERT_RED " " _ASSERT_FAILURE " %s:\n", message);
    fprintf(stderr, "Expected ");
    showbuf(expected, elen);
    fprintf(stderr, "\n");
    fprintf(stderr, "Got      ");
    showbuf(actual, alen);
    fprintf(stderr, "\n" _ASSERT_CLEAR);
    exit(1);
  } else {
    fprintf(stderr, _ASSERT_GREEN " " _ASSERT_SUCCESS " %s\n" _ASSERT_CLEAR, message);
  }
}

#endif
