#!/bin/sh

clear

rm -f ./socketServerForwarder
gcc -pthread -o portForwarder socketServerForwarder.c && ./portForwarder $1 $2 $3
