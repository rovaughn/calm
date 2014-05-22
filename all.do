#!/bin/bash -e
deps="main.o buffer.o event.o screen.o state.o field.o"
redo-ifchange $deps
./cc $deps -o calm
