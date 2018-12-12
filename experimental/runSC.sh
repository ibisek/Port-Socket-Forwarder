#!/bin/sh

clear

rm ./socketClient

gcc -pthread -o socketClient socketClient.c && ./socketClient
