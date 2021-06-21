- driver name: monitor
- major number: 242

To run, do

insmod monitor.ko
mknod /dev/monitor c 242 0
rmmod monitor
rm /dev/monitor
