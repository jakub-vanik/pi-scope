#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release -S . -B build &&
make -C build
