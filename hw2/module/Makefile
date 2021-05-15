obj-m = dev_driver.o
CC 		:= arm-none-linux-gnueabi-gcc

KDIR 	:= /work/achroimx_kernel
PWD		:= $(shell pwd)

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf dev_driver
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .dev*