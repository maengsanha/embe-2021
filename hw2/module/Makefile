obj-m   := dev_driver.o

KDIR    := /work/achroimx_kernel
PWD     := $(shell pwd)

all: setup driver

setup:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

driver:
	arm-none-linux-gnueabi-gcc -static -o dev_driver dev_driver.c

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf dev_driver
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .cmd