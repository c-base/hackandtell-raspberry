#!/bin/sh

cd /sys/class/gpio
echo 2 > export
cd gpio2
echo out > direction
echo 0 > value
