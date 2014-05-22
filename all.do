#!/bin/bash -e
find tests -name '*-test.c' -exec echo "{}.tested" \; | xargs redo-ifchange
deps="main.o buffer.o event.o screen.o state.o field.o utf8.o"
redo-ifchange $deps
./cc $deps -o calm
