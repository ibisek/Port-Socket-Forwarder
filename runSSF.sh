#!/bin/sh

clear

rm ./socketServerForwarder

gcc -pthread -o socketServerForwarder socketServerForwarder.c && ./socketServerForwarder




