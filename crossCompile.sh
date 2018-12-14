#!/bin/bash 

. /bin/openwrt.config 

mipsel-openwrt-linux-gcc -o socketServerForwarder ./socketServerForwarder.c 
