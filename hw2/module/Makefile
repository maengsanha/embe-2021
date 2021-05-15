obj-m   := dev_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

all: driver app

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o dev_driver dev_driver.c

install:
	cp -a dev_driver.ko /nfsroot
	cp -a dev_driver /nfsroot

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf dev_driver
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*