/*
 * Embedded System Software, 2021
 *
 * module/dev_driver.c - timer deivce driver
 */
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>

#include "args.h"
#include "fpga_text_lcd_util.h"
#include "fpga_dot_font.h"

#define FND_ADDRESS        0x08000004
#define LED_ADDRESS        0x08000016
#define TEXT_LCD_ADDRESS   0x08000090
#define DOT_MATRIX_ADDRESS 0x08000210
#define DEV_DRIVER         "/dev/dev_driver"
#define DEV_MAJOR          242

static unsigned char *fnd_addr;
static unsigned char *led_addr;
static unsigned char *text_lcd_addr;
static unsigned char *dot_matrix_addr;

struct args *param;

static int __init timer_init();
static void __exit timer_exit();

/**
 * get_init_val - returns initial value of @param
 *
 * @param: command line argument from user program
 */
static inline int get_init_val(struct args *param) {
  return 1000 < param->init ? param->init/1000
        : 100 < param->init ? param->init/100
         : 10 < param->init ? param->init/10
                            : param->init;
}

///////////////////////////////////////////////////////// FND Device /////////////////////////////////////////////////////////

/**
 * fnd_write - writes @data to @fnd_addr
 *
 * @fnd_addr: the address of FND device
 * @data:     the data to write to @fnd_addr
 */
static inline void fnd_write(unsigned char *fnd_addr, const char *data) {
  unsigned char  value[4];
  unsigned short s_value;
  strcpy(value, data);
  s_value = (value[0] << 12) | (value[1] << 8) | (value[2] << 4) | value[3];
  outw(s_value, (unsigned int)fnd_addr);
}

/**
 * fnd_init - initializes @fnd_addr to @init of @param
 *
 * @fnd_addr: the address of FND device
 * @param:    command line argument from user program
 */
static inline void fnd_init(unsigned char *fnd_addr, struct args *param) {
  unsigned char value[4];
  int val  = param->init;
  value[0] = val/1000;
  val      %= 1000;
  value[1] = val/100;
  val      %= 100;
  value[2] = val/10;
  val      %= 10;
  value[3] = val;
  printk("fnd_init: %s\n", value);
  fnd_write(fnd_addr, value);
}

/**
 * fnd_exit - initializes @fnd_addr to zero value
 *
 * @fnd_addr: the address of FND device
 */
static inline void fnd_exit(unsigned char *fnd_addr) {
  unsigned char value[4] = {0x00, 0x00, 0x00, 0x00};
  fnd_write(fnd_addr, value);
}

///////////////////////////////////////////////////////// LED Device /////////////////////////////////////////////////////////

/**
 * led_write - writes @data to @led_addr
 *
 * @led_addr: the address of LED device
 * @data:     the data to write to @led_addr
 */
static inline void led_write(unsigned char *led_addr, unsigned short data) { outw(data, (unsigned int)led_addr); }

/**
 * led_init - initializes @led_addr to @init of @param
 *
 * @led_addr: the address of LED device
 * @param:    command line argument from user program
 */
static inline void led_init(unsigned char *led_addr, struct args *param) {
  switch (get_init_val(param)) {
    case 1:
      led_write(led_addr, (unsigned short)0x80);
      break;
    case 2:
      led_write(led_addr, (unsigned short)0x40);
      break;
    case 3:
      led_write(led_addr, (unsigned short)0x20);
      break;
    case 4:
      led_write(led_addr, (unsigned short)0x10);
      break;
    case 5:
      led_write(led_addr, (unsigned short)0x08);
      break;
    case 6:
      led_write(led_addr, (unsigned short)0x04);
      break;
    case 7:
      led_write(led_addr, (unsigned short)0x02);
      break;
    case 8:
      led_write(led_addr, (unsigned short)0x01);
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
static inline void led_exit(unsigned char *led_addr) { led_write(led_addr, (unsigned short)0x00); }

///////////////////////////////////////////////////////// Text LCD Device /////////////////////////////////////////////////////////

/**
 * text_lcd_write - writes @high and @low to @text_lcd_addr
 *
 * @text_lcd_addr: the address of Text LCD device
 * @high:          the data to write to first line of @text_lcd_addr
 * @low:           the data to write to second line of @text_lcd_addr
 */
static inline void text_lcd_write(unsigned char *text_lcd_addr, const char *high, const char *low) {
  unsigned int i;
  unsigned short s_value;
  unsigned char  value[33];
  strcpy(value, high);
  strcpy(&value[16], low);
  value[32] = 0;

  for (i=0; i<32; i+=2) {
    s_value = (value[i] & 0xFF) << 8 | (value[i+1] & 0xFF);
    outw(s_value, (unsigned int)text_lcd_addr+i);
  }
}

/**
 * text_lcd_init - initializes @text_lcd_addr to @STU_NO and @NAME
 *
 * @text_lcd_addr: the address of Text LCD device
 */
static inline void text_lcd_init(unsigned char *text_lcd_addr) { text_lcd_write(text_lcd_addr, STU_NO, NAME); }

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 *
 * @text_lcd_addr: the address of Text LCD device
 */
static inline void text_lcd_exit(unsigned char *text_lcd_addr) {
  char high[16];
  char low[16];
  memset(high, 0x20, 16);
  memset(low, 0x20, 16);
  text_lcd_write(text_lcd_addr, high, low);
}

///////////////////////////////////////////////////////// Dot Matrix Device /////////////////////////////////////////////////////////

/**
 * dot_matrix_write - writes @data to @dot_matrix_addr
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 * @data:            the data to write to @dot_matrix_addr
 */
static inline void dot_matrix_write(unsigned char *dot_matrix_addr, const char *data) {
  unsigned int i;
  unsigned char value[10];
  unsigned short s_value;
  strcpy(value, data);
  for (i=0; i<10; ++i) {
    s_value = value[i] & 0x7F;
    outw(s_value, (unsigned int)dot_matrix_addr+i*2);
  }
}

/**
 * dot_matrix_init - initializes @dot_matrix_addr to @init of @param
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 * @param:           command line argument from user program
 */
static inline void dot_matrix_init(unsigned char *dot_matrix_addr, struct args *param) { dot_matrix_write(dot_matrix_addr, fpga_number[get_init_val(param)]); }

/**
 * dot_matrix_exit - initializes @dot_matrix_addr to blank
 *
 * @dot_matrix_addr: the address of Dot Matrix device
 */
static inline void dot_matrix_exit(unsigned char *dot_matrix_addr) { dot_matrix_write(dot_matrix_addr, fpga_number[0]); }

///////////////////////////////////////////////////////// Timer Device /////////////////////////////////////////////////////////

/**
 * timer_open - device driver opening event
 *
 * @minode: not used
 * @mfile:  not used
 */
static int timer_open(struct inode *minode, struct file *mfile) {
  printk("%s open\n", DEV_DRIVER);

  // map devices to kernel space
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
static int timer_release(struct inode *minode, struct file *mfile) {
  printk("%s close\n", DEV_DRIVER);

  // fnd_exit(fnd_addr);
  // led_exit(led_addr);
  // text_lcd_exit(text_lcd_addr);
  // dot_matrix_exit(dot_matrix_addr);

  // unmap devices
  iounmap(fnd_addr);
  iounmap(led_addr);
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
static int __init timer_init() {
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
static void __exit timer_exit() {
	unregister_chrdev(DEV_MAJOR, DEV_DRIVER);
  printk("%s exit\n", DEV_DRIVER);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanha.maeng@gmail.com");
