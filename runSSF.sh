#!/bin/sh

clear

rm -f ./socketServerForwarder

gcc -pthread -o socketServerForwarder socketServerForwarder.c && ./socketServerForwarder $1 $2 $3
