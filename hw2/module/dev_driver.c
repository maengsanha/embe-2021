/*
 * Embedded System Software, 2021
 *
 * module/dev_driver.c - timer deivce driver
 */
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "args.h"
#include "fpga_led.h"
#include "fpga_fnd.h"
#include "fpga_text_lcd.h"
#include "fpga_dot.h"

#define LED_ADDRESS        0x08000016
#define FND_ADDRESS        0x08000024
#define TEXT_LCD_ADDRESS   0x08000090
#define DOT_MATRIX_ADDRESS 0x08000210
#define DEV_DRIVER         "/dev/dev_driver"
#define DEV_MAJOR          242

static unsigned char *led_addr;
static unsigned char *fnd_addr;
static unsigned char *text_lcd_addr;
static unsigned char *dot_matrix_addr;

struct args *param;

/**
 * timer_open - device driver opening event
 *
 * @minode: not used
 * @mfile:  not used
 */
static int timer_open(struct inode *minode, struct file *mfile) {
  printk("%s open\n", DEV_DRIVER);

  // map devices to kernel space
  led_addr        = ioremap(LED_ADDRESS, 1);
  fnd_addr        = ioremap(FND_ADDRESS, 4);
  text_lcd_addr   = ioremap(TEXT_LCD_ADDRESS, 32);
  dot_matrix_addr = ioremap(DOT_MATRIX_ADDRESS, 10);

  return 0;
}

/**
 * timer_release - device driver closing event
 *
 * @minode: not used
 * @mfile:  not used
 */
static int timer_release(struct inode *minode, struct file *mfile) {
  printk("%s close\n", DEV_DRIVER);

  // led_exit(led_addr);
  // fnd_exit(fnd_addr);
  // text_lcd_exit(text_lcd_addr);
  // dot_matrix_exit(dot_matrix_addr);

  // unmap devices
  iounmap(led_addr);
  iounmap(fnd_addr);
  iounmap(text_lcd_addr);
  iounmap(dot_matrix_addr);

  return 0;
}

/**
 * timer_ioctl - ioctl event
 *
 * @filp:  not used
 * @cmd:   command
 * @arg:   parameters delivered from user-level ioctl
 */
static long timer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  switch (_IOC_NR(cmd)) {
    case 0:
      // initialize parameters and devices using @arg
      param = (struct args *)arg;
      led_init(led_addr, param);
      fnd_init(fnd_addr, param);
      text_lcd_init(text_lcd_addr);
      dot_matrix_init(dot_matrix_addr, param);
      break;
    case 1:
      // run timer application
      printk("run app\n");
      break;
    default:
      // no such case
      break;
  }

  return 0;
}

// register file operations
static struct file_operations timer_fops = {
  .open           = timer_open,
  .release        = timer_release,
  .unlocked_ioctl = timer_ioctl,
};

/**
 * timer_init - registers device (executed on insmod)
 */
int __init timer_init() {
	printk("%s init\n", DEV_DRIVER);

  if (register_chrdev(DEV_MAJOR, DEV_DRIVER, &timer_fops) < 0) {
		printk("error registering %s\n", DEV_DRIVER);
		return DEV_MAJOR;
	}
	
  printk("dev_file: %s, major: %d\n", DEV_DRIVER, DEV_MAJOR);

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
