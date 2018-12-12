#!/bin/sh

clear

#export LDFLAGS=" -pthread -lpthread"

rm ./socketServerThreaded

gcc -pthread -o socketServerThreaded socketServerThreaded.c && ./socketServerThreaded
#gcc -lpthread -o socketServerThreaded socketServerThreaded.c && ./socketServerThreaded




