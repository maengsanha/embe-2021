obj-m   := monitor.o
KDIR    := /work/achroimx_kernel
PWD     := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@rm -rf *.ko
	@rm -rf *.mod.*
	@rm -rf *.o
	@rm -rf monitor
	@rm -rf Module.symvers
	@rm -rf modules.order
	@rm -rf .tmp*
	@rm -rf .dev*
	@rm -rf .monitor*
