#!/bin/bash

cat /sys/block/hda/queue/scheduler
echo look > /sys/block/hda/queue/scheduler
cat /sys/block/hda/queue/scheduler
sync
dd if=/dev/urandom of=./test_data_1 bs=1024 count=8192
sync
cp test_data_1 test_data_2
sync
diff test_data_1 test_data_2
if [[ $? != 0 ]]; then
	echo "Test failed!"
else
	echo "Test passed!"
fi
rm test_data_1 test_data_2
