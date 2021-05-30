obj-m   := stopwatch.o
KDIR    := /work/achroimx_kernel
PWD     := $(shell pwd)

all: setup driver

setup:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

driver:
	arm-none-linux-gnueabi-gcc -static -o stopwatch stopwatch.c

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf stopwatch
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .dev*
