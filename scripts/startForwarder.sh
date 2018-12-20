#!/bin/sh /etc/rc.common

START=96
STOP=4

start(){
 #echo "starting.." >> /tmp/forwarder.log
 sleep 20; /opt/portForwarder.bin 80 example.com 80 &
 #echo "started" >> /tmp/forwarder.log
}

