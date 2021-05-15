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
#include "fpga_text_lcd_util.h"
#include "fpga_dot_font.h"

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
 * get_init_val - returns initial value of @param
 *
 * @param: command line argument from user program
 */
inline unsigned int get_init_val(struct args *param) {
  unsigned int init = param->init;
  if (0 < init/1000) return init/1000;
  if (0 < init/100)  return init/100;
  if (0 < init/10)   return init/10;
  return init;
}

/**
 * led_init - initializes @led_addr to @init of @param
 *
 * @led_addr: the address of LED device
 * @param:    command line argument from user program
 */
inline void led_init(unsigned char *led_addr, struct args *param) {
  switch (get_init_val(param)) {
    case 1:
      *led_addr = 0x80;
      break;
    case 2:
      *led_addr = 0x40;
      break;
    case 3:
      *led_addr = 0x20;
      break;
    case 4:
      *led_addr = 0x10;
      break;
    case 5:
      *led_addr = 0x08;
      break;
    case 6:
      *led_addr = 0x04;
      break;
    case 7:
      *led_addr = 0x02;
      break;
    case 8:
      *led_addr = 0x01;
      break;
    default:
      // no such case
      break;
  }
}

/**
 * led_exit - initializes @led_addr to zero value
 *
 * @led_addr: the address of LED device
 */
inline void led_exit(unsigned char *led_addr) { *led_addr = 0x00; }

/**
 * fnd_init - initializes @fnd_addr to @init of @param
 *
 * @fnd_addr: the address of FND device
 * @param:    command line argument from user program
 */
inline void fnd_init(unsigned char *fnd_addr, struct args *param) {
  unsigned char val = param->init;
  fnd_addr[0]       = val/1000;
  val               %= 1000;
  fnd_addr[1]       = val/100;
  val               %= 100;
  fnd_addr[2]       = val/10;
  val               %= 10;
  fnd_addr[3]       = val;
}

/**
 * fnd_exit - initializes @fnd_addr to zero value
 *
 * @fnd_addr: the address of FND device
 */
inline void fnd_exit(unsigned char *fnd_addr) {
  fnd_addr[0] = 0x00;
  fnd_addr[1] = 0x00;
  fnd_addr[2] = 0x00;
  fnd_addr[3] = 0x00;
}

/**
 * text_lcd_init - initializes @text_lcd_addr to @STU_NO and @NAME
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_init(unsigned char *text_lcd_addr) {
  unsigned int i;
  for (i=15; 0 <= i; --i) {
    text_lcd_addr[i]    = STU_NO[i];
    text_lcd_addr[i+16] = NAME[i];
  }
}

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_exit(unsigned char *text_lcd_addr) {
  unsigned int i;
  for (i=31; 0 <= i; --i) text_lcd_addr[i] = 0x20;
}

/**
 * dot_matrix_init - initializes @dot_matrix_addr to @init of @param
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 * @param:           command line argument from user program
 */
inline void dot_matrix_init(unsigned char *dot_matrix_addr, struct args *param) {
  unsigned char number[10] = fpga_number[get_init_val(param)];
  unsigned int i;
  for (i=9; 0 <= i; --i) dot_matrix_addr[i] = number[i];
}

/**
 * dot_matrix_exit - initializes @dot_matrix_addr to zero value
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 */
inline void dot_matrix_exit(unsigned char *dot_matrix_addr) {
  unsigned int i;
  for (i=9; 0 <= i; --i) dot_matrix_addr[i] = 0x00;
}

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
