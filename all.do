#!/bin/bash -e
find tests -name '*-test.c' -exec echo "{}.tested" \; | xargs redo-ifchange
examples=`ls examples`
deps="buffer.o event.o screen.o state.o field.o utf8.o dots.o"
redo-ifchange $deps
find examples -name '*.c' | xargs redo-ifchange

for example in $examples
do
  ./cc $deps examples/$example -o example-${example%.*} -lm
done

