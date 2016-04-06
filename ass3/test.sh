#!/bin/sh
insmod ebd.ko
echo $?

mkfs /dev/ebd0
echo $?
mount /dev/ebd0 /mnt
echo $?

echo 'hello encryption!' > /mnt/hello

cat /mnt/hello

umount /mnt

rmmod ebd.ko
echo $?
