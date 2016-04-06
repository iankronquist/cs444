Notes
=====

Edit this variable in the kernel .config
CONFIG_CROSS_COMPILE="-kronquii-hw1"


qemu-img create  core-image-lsb-sdk-qemux86.ext3 512M
mkfs.ext3 core-image-lsb-sdk-qemux86.ext3

Copying files to the vm:
Redirect host 7222 -> qemu 22.
$ scp -P 7222 -r ass3 root@localhost:/home/root
