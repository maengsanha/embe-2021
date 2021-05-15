CC 		 = arm-none-linux-gnueabi-gcc
CFLAGS = -static
TARGET = dev_driver
OBJS	 = dev_driver.o fpga_led.o fpga_fnd.o fpga_text_lcd.o fpga_dot_matrix.o

obj-m  := dev_driver.o

KDIR   := /work/achroimx_kernel
PWD    := $(shell pwd)

all: setup $(TARGET)

setup:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o dev_driver dev_driver.o fpga_led.o fpga_fnd.o fpga_text_lcd.o fpga_dot_matrix.o

dev_driver.o: dev_driver.c
	$(CC) $(CFLAGS) -o dev_driver.o dev_driver.c

fpga_led.o: fpga_led.c
	$(CC) $(CFLAGS) -o fpga_led.o fpga_led.c

fpga_fnd.o: fpga_fnd.c
	$(CC) $(CFLAGS) -o fpga_fnd.o fpga_fnd.c

fpga_text_lcd.o: fpga_text_lcd.c
	$(CC) $(CFLAGS) -o fpga_text_lcd.o fpga_text_lcd.c

fpga_dot_matrix.o: fpga_dot_matrix.c
	$(CC) $(CFLAGS) -o fpga_dot_matrix.o fpga_dot_matrix.c

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf dev_driver
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .dev*