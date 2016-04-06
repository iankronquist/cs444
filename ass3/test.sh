#!/bin/sh
insmod ebd.ko
echo $?

mkfs /dev/ebd0
echo $?
mnt /dev/ebd0 /mnt
echo $?

echo 'hello encryption!' > /mnt/hello

cat /mnt/hello

rmmod ebd.ko
echo $?
