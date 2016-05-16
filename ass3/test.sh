#!/bin/sh

TEST_PATH=/tmp/ebdtest/
EXIT_CODE=0

assert_success () {
	echo $@
	$@
	code=$?
	if [ $code -eq 0 ] ; then
		echo -e "\e[32mTest passed!\e[00m"
	else
		echo $code
		EXIT_CODE=$code
		echo -e "\e[31mTest failed!\e[00m"
	fi
}

assert_failure () {
	echo $@
	$@
	code=$?
	if [ $code -eq 0 ] ; then
		EXIT_CODE=$code
		echo -e "\e[31mTest failed!\e[00m"
	else
		echo -e "\e[32mTest passed!\e[00m"
	fi
}

assert_key () {
	echo "Checking that the key is $1."
	if [ `cat /sys/block/ebd0/key` = $1 ] ; then
		echo -e "\e[32mTest passed!\e[00m"
	else
		EXIT_CODE=$code
		echo -e "\e[31mTest failed!\e[00m"
	fi
}

rm -rf $TEST_PATH
mkdir $TEST_PATH

assert_success insmod ebd.ko

shred /dev/ebd0

assert_success mkfs.ext2 /dev/ebd0

assert_success mount /dev/ebd0 $TEST_PATH

echo 'hello encryption!' > $TEST_PATH/hello

assert_failure grep -a 'hello' /dev/ebd0

cat $TEST_PATH/hello

assert_success umount $TEST_PATH

assert_success rmmod ebd.ko

rm -rf $TEST_PATH

exit $EXIT_CODE
