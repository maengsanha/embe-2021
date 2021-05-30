- driver name: stopwatch
- major number: 242

To run, do

insmod stopwatch.ko
mknod /dev/stopwatch c 242 0
./app
rmmod stopwatch
rm /dev/stopwatch
