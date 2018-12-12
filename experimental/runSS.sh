#!/bin/sh

clear

rm ./socketServer

gcc -pthread -o socketServer socketServer.c && ./socketServer




