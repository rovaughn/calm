#!/bin/bash -e
redo-ifchange main.o buffer.o event.o screen.o state.o
./cc main.o buffer.o event.o screen.o state.o -o calm
