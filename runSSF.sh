#!/bin/sh

clear

rm -f ./portForwarder.bin
gcc -pthread -O3 -o portForwarder.bin socketServerForwarder.c && ./portForwarder.bin $1 $2 $3
