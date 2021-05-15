/*
 * Embedded System Software, 2021
 *
 * module/dev_driver.c - timer deivce driver
 */
#include <unistd.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>

#define FND_DEVICE        "/dev/fpga_fnd"
#define LED_DEVICE        "/dev/fpga_led"
#define TEXT_LCD_DEVICE   "/dev/fpga_text_lcd"
#define DOT_MATRIX_DEVICE "/dev/fpga_dot"
#define DEV_DRIVER        "/dev/dev_driver"
#define DEV_MAJOR         242

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

int fnd_fd, led_fd, text_lcd_fd, dot_fd;

/**
 * timer_open - device driver opening event
 *
 * @minode: not used
 * @mfile:  not used
 */
int timer_open(struct inode *minode, struct file *mfile) {
  printk("%s open\n", DEV_DRIVER);

  // open devices
  if ((fnd_fd = open(FND_DEVICE, O_RDWR)) < 0) {
    printk("open %s failed\n", FND_DEVICE);
    return fnd_fd;
  }
  if ((led_fd = open(LED_DEVICE, O_RDWR)) < 0) {
    printk("open %s failed\n", LED_DEVICE);
    close(fnd_fd);
    return led_fd;
  }
  if ((text_lcd_fd = open(TEXT_LCD_DEVICE, O_WRONLY)) < 0) {
    printk("open %s failed\n", TEXT_LCD_DEVICE);
    close(fnd_fd);
    close(led_fd);
    return text_lcd_fd;
  }
  if ((dot_fd = open(DOT_MATRIX_DEVICE, O_WRONLY)) < 0) {
    printk("open %s failed\n", DOT_MATRIX_DEVICE);
    close(fnd_fd);
    close(led_fd);
    close(text_lcd_fd);
    return dot_fd;
  }

  // TODO

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
  close(fnd_fd);
  close(led_fd);
  close(text_lcd_fd);
  close(dot_fd);

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
 * timer_init - executed when insmod
 */
int __init timer_init() {
	printk("timer init\n");

  int major;

  if ((major = register_chrdev(DEV_MAJOR, DEV_DRIVER, &timer_fops)) < 0) {
		printk("error %d\n", major);
		return major;
	}
	
  printk("dev_file: %s, major: %d\n", DEV_DRIVER, major);

  return 0;
}

/**
 * timer_exit - executed when rmmod
 */
void __exit timer_exit() {
	printk("timer exit\n");

	unregister_chrdev(DEV_MAJOR, DEV_DRIVER);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanha.maeng@gmail.com");
