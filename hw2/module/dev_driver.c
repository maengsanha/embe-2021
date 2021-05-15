/*
 * Embedded System Software, 2021
 *
 * module/dev_driver.c - timer deivce driver
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <asm-generic/io.h>

#define FND_ADDRESS        0x08000024
#define LED_ADDRESS        0x08000016
#define TEXT_LCD_ADDRESS   0x08000090
#define DOT_MATRIX_ADDRESS 0x08000210

#define FND_DEVICE        "/dev/fpga_fnd"
#define LED_DEVICE        "/dev/fpga_led"
#define TEXT_LCD_DEVICE   "/dev/fpga_text_lcd"
#define DOT_MATRIX_DEVICE "/dev/fpga_dot"
#define DEV_DRIVER        "/dev/dev_driver"
#define DEV_MAJOR         242

static unsigned char *fnd_addr;
static unsigned char *led_addr;
static unsigned char *text_lcd_addr;
static unsigned char *dot_matrix_addr;

/**
 * struct args - same as user-level struct args
 *
 * See app/args.h
 */
static struct args {
  int interval;
  int cnt;
  int init;
};

/**
 * timer_open - device driver opening event
 *
 * @minode: not used
 * @mfile:  not used
 */
int timer_open(struct inode *minode, struct file *mfile) {
  printk("%s open\n", DEV_DRIVER);

  // open devices
  fnd_addr        = ioremap(FND_ADDRESS, 0x04);
  led_addr        = ioremap(LED_ADDRESS, 0x01);
  text_lcd_addr   = ioremap(TEXT_LCD_ADDRESS, 0x32);
  dot_matrix_addr = ioremap(DOT_MATRIX_ADDRESS, 0x10);

  return 0;
}

/**
 * timer_release - device driver closing event
 *
 * @minode: not used
 * @mfile:  not used
 */
int timer_release(struct inode *minode, struct file *mfile) {
  printk("%s close\n", DEV_DRIVER);

  // close devices
  iounmap(fnd_addr);
  iounmap(led_addr);
  iounmap(text_lcd_addr);
  iounmap(dot_matrix_addr);

  return 0;
}

/**
 * timer_ioctl - ioctl event
 *
 * @minode: not used
 * @mfile:  not used
 * @cmd:    command
 * @args:   parameters delivered from user-level ioctl
 */
int timer_ioctl(struct inode *minode, struct file *mfile, unsigned int cmd, unsigned long args) {
  // TODO

  return 0;
}

// register file operations
static struct file_operations timer_fops = {
  .open    = timer_open,
  .release = timer_release,
  .ioctl   = timer_ioctl,
};

/**
 * timer_init - registers device (executed on insmod)
 */
int __init timer_init() {
	printk("%s init\n", DEV_DRIVER);

  int major;

  if ((major = register_chrdev(DEV_MAJOR, DEV_DRIVER, &timer_fops)) < 0) {
		printk("error %d\n", major);
		return major;
	}
	
  printk("dev_file: %s, major: %d\n", DEV_DRIVER, major);

  return 0;
}

/**
 * timer_exit - unregisters device (executed on rmmod)
 */
void __exit timer_exit() {
	unregister_chrdev(DEV_MAJOR, DEV_DRIVER);
  printk("%s exit\n", DEV_DRIVER);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanha.maeng@gmail.com");
