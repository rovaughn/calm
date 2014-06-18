#!/bin/bash -e
find tests -name '*-test.c' -exec echo "{}.tested" \; | xargs redo-ifchange
deps="event.o screen.o utf8.o dots.o test.o wcwidth.o"
redo-ifchange $deps
./cc $deps -o test

