/*
 * Embedded System Software, 2021
 *
 * module/dev_driver.c - timer deivce driver
 */
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

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

static struct args param;
static struct timer_list timer;

static int curr_val;
static int fnd_pos;
static int fnd_rot;
static int high_pos;
static int high_up;
static int low_pos;
static int low_up;

/**
 * get_init_val - returns initial value of @param
 */
static inline int get_init_val() {
  return 1000 < param.init ? param.init/1000
        : 100 < param.init ? param.init/100
         : 10 < param.init ? param.init/10
                           : param.init;
}

/**
 * get_init_pos - returns initial position of @param
 */
static inline int get_init_pos() {
  return 1000 < param.init ? 3
        : 100 < param.init ? 2
         : 10 < param.init ? 1
                           : 0;
}

///////////////////////////////////////////////////////// FND Device /////////////////////////////////////////////////////////

/**
 * fnd_write - writes @a@b@c@d to @fnd_addr
 *
 * @a: the first digit to write to @fnd_addr
 * @b: the second digit to write to @fnd_addr
 * @c: the third digit to write to @fnd_addr
 * @d: the fourth digit to write to @fnd_addr
 */
static inline void fnd_write(int a, int b, int c, int d) {
  unsigned short s_value;
  s_value = (a << 12) | (b << 8) | (c << 4) | d;
  outw(s_value, (unsigned int)fnd_addr);
}

/**
 * fnd_init - initializes @fnd_addr to @init of @param
 */
static inline void fnd_init() {
  int first  = param.init/1000;
  int second = param.init/100%10;
  int third  = param.init/10%10;
  int fourth = param.init%10;
  fnd_write(first, second, third, fourth);
}

/**
 * fnd_exit - initializes @fnd_addr to zero value
 */
static inline void fnd_exit() { fnd_write(0, 0, 0, 0); }

///////////////////////////////////////////////////////// LED Device /////////////////////////////////////////////////////////

/**
 * led_write - writes @data to @led_addr
 *
 * @led_addr: the address of LED device
 * @data:     the data to write to @led_addr
 */
static inline void led_write(unsigned short data) { outw(data, (unsigned int)led_addr); }

/**
 * led_init - initializes @led_addr to @init of @param
 */
static inline void led_init() {
  switch (get_init_val()) {
    case 1:
      led_write((unsigned short)0x80);
      break;
    case 2:
      led_write((unsigned short)0x40);
      break;
    case 3:
      led_write((unsigned short)0x20);
      break;
    case 4:
      led_write((unsigned short)0x10);
      break;
    case 5:
      led_write((unsigned short)0x08);
      break;
    case 6:
      led_write((unsigned short)0x04);
      break;
    case 7:
      led_write((unsigned short)0x02);
      break;
    case 8:
      led_write((unsigned short)0x01);
      break;
    default:
      // no such case
      break;
  }
}

/**
 * led_exit - initializes @led_addr to zero value
 */
static inline void led_exit() { led_write((unsigned short)0x00); }

///////////////////////////////////////////////////////// Text LCD Device /////////////////////////////////////////////////////////

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
 * text_lcd_init - initializes @text_lcd_addr to @STU_NO and @NAME
 */
static inline void text_lcd_init() { text_lcd_write(STU_NO, NAME); }

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 */
static inline void text_lcd_exit() {
  char high[16];
  char low[16];
  memset(high, 0x20, 16);
  memset(low, 0x20, 16);
  text_lcd_write(high, low);
}

///////////////////////////////////////////////////////// Dot Matrix Device /////////////////////////////////////////////////////////

/**
 * dot_matrix_write - writes @data to @dot_matrix_addr
 *
 * @data: the data to write to @dot_matrix_addr
 */
static inline void dot_matrix_write(const char *data) {
  int i;
  unsigned char value[10];
  unsigned short int s_value;
  strncpy(value, data, 10);
  for (i=0; i<10; i++) {
    s_value = value[i] & 0x7F;
    outw(s_value, (unsigned int)dot_matrix_addr+i*2);
  }
}

/**
 * dot_matrix_init - initializes @dot_matrix_addr to @init of @param
 */
static inline void dot_matrix_init() { dot_matrix_write(fpga_number[get_init_val()]); }

/**
 * dot_matrix_exit - initializes @dot_matrix_addr to blank
 */
static inline void dot_matrix_exit() { dot_matrix_write(fpga_number[0]); }

///////////////////////////////////////////////////////// Timer Device /////////////////////////////////////////////////////////

/**
 * timer_blink - fetches devices every TIMER_INTERVAL/10 HZ for TIMER_CNT
 */
static void timer_blink(unsigned long timeout) {
  struct args *data = (struct args *)timeout;

  if (--data->cnt < 1) return;

  // fetch conditions
  curr_val = curr_val == 8 ? 1 : curr_val + 1;
  fnd_pos  = fnd_rot == 7 ? (fnd_pos + 3) % 4 : fnd_pos;
  fnd_rot  = (fnd_rot + 1) % 8;
  if (high_up) {
    if (high_pos == 7) {
      high_up = 0;
    }
  } else {
    if (high_pos == 0) {
      high_up = 1;
    }
  }
  if (low_up) {
    if (low_pos == 5) {
      low_up = 0;
    }
  } else {
    if (low_pos == 0) {
      low_up = 1;
    }
  }
  high_pos = high_up ? high_pos + 1 : high_pos - 1;
  low_pos  = low_up ? low_pos + 1 : low_pos - 1;

  // fetch FND
  switch (fnd_pos) {
    case 3:
      fnd_write(curr_val, 0, 0, 0);
      break;
    case 2:
      fnd_write(0, curr_val, 0, 0);
      break;
    case 1:
      fnd_write(0, 0, curr_val, 0);
      break;
    case 0:
      fnd_write(0, 0, 0, curr_val);
      break;
    default:
      // no such case
      break;
  }

  // fetch LED
  switch (curr_val) {
    case 1:
      led_write((unsigned short)0x80);
      break;
    case 2:
      led_write((unsigned short)0x40);
      break;
    case 3:
      led_write((unsigned short)0x20);
      break;
    case 4:
      led_write((unsigned short)0x10);
      break;
    case 5:
      led_write((unsigned short)0x08);
      break;
    case 6:
      led_write((unsigned short)0x04);
      break;
    case 7:
      led_write((unsigned short)0x02);
      break;
    case 8:
      led_write((unsigned short)0x01);
      break;
    default:
      // no such case
      break;
  }

  // fetch Dot Matrix
  dot_matrix_write(fpga_number[curr_val]);

  // fetch Text LCD
  char high[16];
  char low[16];
  memset(high, 0x20, 16);
  memset(low, 0x20, 16);
  strncpy(&high[high_pos], STU_NO, 9);
  strncpy(&low[low_pos], NAME, 11);
  text_lcd_write(high, low);

  timer.expires  = get_jiffies_64() + (param.interval * (HZ/10));
  timer.data = (unsigned long)&param;
  timer.function = timer_blink;
  add_timer(&timer);
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
      copy_from_user(&param, (void __user *)arg, sizeof(struct args));

      fnd_init();
      led_init();
      text_lcd_init();
      dot_matrix_init();

      curr_val = get_init_val();
      fnd_pos  = get_init_pos();
      fnd_rot  = 0;
      high_pos = 0;
      high_up  = 1;
      low_pos  = 0;
      low_up   = 1;
      break;
    case 1:
      // run timer application
      del_timer_sync(&timer);
      timer.expires  = get_jiffies_64() + (param.interval * (HZ/10));
      timer.data     = (unsigned long)&param;
      timer.function = timer_blink;
      add_timer(&timer);
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

  init_timer(&timer);

  return 0;
}

/**
 * timer_exit - unregisters device (executed on rmmod)
 */
static void __exit timer_exit() {
  fnd_exit();
  led_exit();
  text_lcd_exit();
  dot_matrix_exit();

  // unmap devices
  del_timer_sync(&timer);
  iounmap(fnd_addr);
  iounmap(led_addr);
  iounmap(text_lcd_addr);
  iounmap(dot_matrix_addr);
  unregister_chrdev(DEV_MAJOR, DEV_DRIVER);
  printk("%s exit\n", DEV_DRIVER);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("msh0117@sogang.ac.kr");
