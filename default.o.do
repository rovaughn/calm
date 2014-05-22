#!/bin/bash -e
redo-ifchange $2.c
./cc -g -Wall -Werror -Wextra -MD -MF $2.d -c -o $3 $2.c
read DEPS <$2.d
rm $2.d
redo-ifchange ${DEPS#*:}
