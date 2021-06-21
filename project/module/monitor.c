/*
 * Embedded System Software, 2021
 *
 * monitor.c - system information monitor device driver
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "sysinfo.h"

#define TEXT_LCD_ADDRESS  0x08000090
#define DEVICE_DRIVER     "/dev/monitor"
#define DEVICE_MAJOR      242

static unsigned char *text_lcd_addr;

/////////////////////////////////////////// Text LCD Device ///////////////////////////////////////////

/**
 * text_lcd_write - writes @high and @low to @text_lcd_addr
 *
 * @high: the data to write to first line of @text_lcd_addr
 * @low:  the data to write to second line of @text_lcd_addr
 */
static inline void text_lcd_write(const char *high, const char *low) {
  int i;
  unsigned short int s_value = 0;
  unsigned char  value[33];
  strncpy(value, high, 16);
  strncpy(&value[16], low, 16);
  value[32] = 0;

  for (i=0; i<32; i++) {
    s_value = (value[i] & 0xFF) << 8 | (value[i+1] & 0xFF);
    outw(s_value, (unsigned int)text_lcd_addr+i);
    i++;
  }
}

/**
 * text_lcd_init - initializes @text_lcd_addr to zero value
 */
static inline void text_lcd_init() {
  char high[16];
  char low[16];
  memset(high, 0x20, 16);
  memset(low, 0x20, 16);
  text_lcd_write(high, low);
}

/////////////////////////////////////////// Monitor Device ///////////////////////////////////////////

/**
 * monitor_open - initializes @text_lcd_addr (executed on open)
 *
 * @inodp: not used
 * @filp:  not used
 */
static int monitor_open(struct inode *inodp, struct file *filp) {
  printk("%s open\n", DEVICE_DRIVER);

  text_lcd_init();

  return 0;
}

/**
 * monitor_release - initializes @text_lcd_addr (executed on close)
 *
 * @inodp: not used
 * @filp:  not used
 */
static int monitor_release(struct inode *inodp, struct file *filp) {
  printk("%s close\n", DEVICE_DRIVER);

  text_lcd_init();

  return 0;
}

/**
 * monitor_write - displays resource usage on Text LCD (executed on write)
 *
 * @filp:  not used
 * @buf:   delivered buffer
 * @size:  size of @buf
 * @f_ops: not used
 */
static int monitor_write(struct file *filp, const char *buf, size_t size, loff_t *f_ops) {
  struct sys_info_t info;
  copy_from_user(&info, (void __user *)buf, sizeof(struct sys_info_t));

  char high[16];
  char low[16];
  memset(high, 0x20, 16);
  memset(high, 0x00, info.user_usage);
  memset(low, 0x20, 16);
  memset(low, 0x00, info.sys_usage);

  text_lcd_write(high, low);

  return 0;
}

// register file operations
static struct file_operations monitor_fops = {
  .open = monitor_open,
  .release = monitor_release,
  .write = monitor_write,
};

/**
 * monitor_init - registers device (executed on insmod)
 */
static int __init monitor_init() {
  printk("%s init\n", DEVICE_DRIVER);

  if (register_chrdev(DEVICE_MAJOR, DEVICE_DRIVER, &monitor_fops) < 0) {
    printk("error registering %s\n", DEVICE_DRIVER);
    return 0;
  }

  text_lcd_addr = ioremap(TEXT_LCD_ADDRESS, 0x32);

  printk("dev_file: %s, major: %d\n", DEVICE_DRIVER, DEVICE_MAJOR);

  return DEVICE_MAJOR;
}

/**
 * monitor_exit - unregisters device (executed on rmmod)
 */
void __exit monitor_exit() {
  iounmap(text_lcd_addr);
  unregister_chrdev(DEVICE_MAJOR, DEVICE_DRIVER);

  printk("%s exit\n", DEVICE_DRIVER);
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("msh0117@sogang.ac.kr");
