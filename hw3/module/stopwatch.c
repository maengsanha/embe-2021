/*
 * Embedded System Software, 2021
 *
 * stopwatch.c - stop watch device drvier
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/gpio.h>
#include <mach/gpio.h>
#include <asm/uaccess.h>

#include "stopwatch.h"

#define FND_ADDRESS   0x08000004
#define DEVICE_DRIVER "/dev/stopwatch"

static int                     stopwatch_major = 242;
static int                     stopwatch_minor = 0;
static int                     result;
static unsigned char           *fnd_addr;
static dev_t                   stopwatch_dev;
static struct cdev             stopwatch_cdev;
static struct timer_list       timer;
static struct workqueue_struct *workqueue;

static int initialized               = 0;
static int done                      = 0;
static unsigned long exit_count      = 0;
static struct stopwatch_t watch_info = {
  .count   = 0,
  .fnd_val = 0,
  .paused  = 1,
};

wait_queue_head_t wq_head;
DECLARE_WAIT_QUEUE_HEAD(wq_head);

///////////////////////////////////////////////////////////// FND Device /////////////////////////////////////////////////////////////

/**
 * fnd_write - writes @x to @fnd_addr
 *
 * @x: the number to write to @fnd_addr
 */
static inline void fnd_write(int x) {
  int first              = x / 1000;
  int second             = (x % 1000) / 100;
  int third              = (x % 100) / 10;
  int fourth             = x % 10;
  unsigned short s_value = (first << 12) | (second << 8) | (third << 4) | fourth;
  outw(s_value, (unsigned int)fnd_addr);
}

/**
 * fnd_init - initializes @fnd_addr
 */
static inline void fnd_init() { fnd_write(0); }

static inline void fnd_fetch() { fnd_write(watch_info.fnd_val); }

///////////////////////////////////////////////////////////// Stopwatch Device /////////////////////////////////////////////////////////////

/**
 * timer_count - counts stopwatch
 *
 * @arg: stopwatch information
 */
static void timer_count(unsigned long arg) {
  static struct work_struct task;
  struct stopwatch_t *info = (struct stopwatch_t *)arg;

  if (info->paused)
    return;

  info->count++;
  info->fnd_val = info->count/10;

  // Bottom half
  if (initialized == 0) {
    INIT_WORK(&task, fnd_fetch);
    initialized = 1;
  } else {
    PREPARE_WORK(&task, fnd_fetch);
  }

  queue_work(workqueue, &task);

  timer.expires  = get_jiffies_64() + (HZ/10);
  timer.data     = (unsigned long)&watch_info;
  timer.function = timer_count;
  add_timer(&timer);
}

/**
 * home_handler - starts stopwatch
 *
 * @irq:    not used
 * @dev_id: not used
 * @reg:    not used
 */
irqreturn_t home_handler(int irq, void *dev_id, struct pt_regs *reg) {
  printk("HOME\n");

  watch_info.paused = 0;

  del_timer_sync(&timer);
  timer.expires  = get_jiffies_64() + (HZ/10);
  timer.data     = (unsigned long)&watch_info;
  timer.function = timer_count;
  add_timer(&timer);

  return IRQ_HANDLED;
}

/**
 * back_handler - pauses stopwatch
 *
 * @irq:    not used
 * @dev_id: not used
 * @reg:    not used
 */
irqreturn_t back_handler(int irq, void *dev_id, struct pt_regs *reg) {
  printk("BACK\n");

  watch_info.paused = 1;

  return IRQ_HANDLED;
}

/**
 * volup_handler - resets stopwatch
 *
 * @irq:    not used
 * @dev_id: not used
 * @reg:    not used
 */
irqreturn_t volup_handler(int irq, void *dev_id, struct pt_regs *reg) {
  printk("VOL+\n");

  watch_info.count   = 0;
  watch_info.fnd_val = 0;
  watch_info.paused  = 1;
  fnd_init();

  return IRQ_HANDLED;
}

/**
 * voldown_handler - stops stopwatch
 *
 * @irq:    not used
 * @dev_id: not used
 * @reg:    not used
 */
irqreturn_t voldown_handler(int irq, void *dev_id, struct pt_regs *reg) {
  printk("VOL-\n");

  if (exit_count == 0) {  // case of falling
    exit_count = get_jiffies_64();
    printk("fall: %lu\n", exit_count);
  } else {  // case of rising
    unsigned long now = get_jiffies_64();
    if (3*HZ <= now - exit_count) {
      watch_info.paused = 1;
      fnd_init();
      done = 1;
      __wake_up(&wq_head, 1, 1, NULL);
      printk("wake up\n");
    } else {
      exit_count = 0;
    }
    printk("rise: %lu\n", now);
  }

  return IRQ_HANDLED;
}

/**
 * stopwatch_open - stopwatch opening event handler
 *
 * @inodp: not used
 * @filp:  not used
 */
static int stopwatch_open(struct inode *inodp, struct file *filp) {
  int irq;

  printk(KERN_ALERT "Open Module\n");
  
  gpio_direction_input(IMX_GPIO_NR(1, 11));
  irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
  request_irq(irq, home_handler, IRQF_TRIGGER_FALLING, "home", 0);

  gpio_direction_input(IMX_GPIO_NR(1, 12));
  irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
  request_irq(irq, back_handler, IRQF_TRIGGER_FALLING, "back", 0);

  gpio_direction_input(IMX_GPIO_NR(2, 15));
  irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
  request_irq(irq, volup_handler, IRQF_TRIGGER_FALLING, "volup", 0);

  gpio_direction_input(IMX_GPIO_NR(5, 14));
  irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
  request_irq(irq, voldown_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", 0);

  return 0;
}

/**
 * stopwatch_release - stopwatch closing event handler
 *
 * @inodp: not used
 * @filp:  not used
 */
static int stopwatch_release(struct inode *inodp, struct file *filp) {
  free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
  free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
  free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
  free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);
  
  printk(KERN_ALERT "Release Module\n");

  return 0;
}

/**
 * stopwatch_write - stopwatch writing event handler
 *
 * @filp:  not used
 * @buf:   not used
 * @count: not used
 * @f_pos: not used
 */
static int stopwatch_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
  if (done == 0) {
    printk("sleep on\n");
    interruptible_sleep_on(&wq_head);
  }

  printk("Module Write\n");
  
  return 0;
}

// register file operations
static struct file_operations stopwatch_fops = {
  .open    = stopwatch_open,
  .release = stopwatch_release,
  .write   = stopwatch_write,
};

/**
 *
 */
static int stopwatch_register_cdev() {
  int error;
  if (stopwatch_major) {
    stopwatch_dev = MKDEV(stopwatch_major, stopwatch_minor);
    error = register_chrdev_region(stopwatch_dev, 1, "stopwatch");
  } else {
    error = alloc_chrdev_region(&stopwatch_dev, stopwatch_minor, 1, "stopwatch");
    stopwatch_major = MAJOR(stopwatch_dev);
  }

  if (error < 0) {
    printk(KERN_WARNING "stopwatch: can't get major %d\n", stopwatch_major);
    return result;
  }

  printk(KERN_ALERT "major number = %d\n", stopwatch_major);
  cdev_init(&stopwatch_cdev, &stopwatch_fops);
  stopwatch_cdev.owner = THIS_MODULE;
  stopwatch_cdev.ops = &stopwatch_fops;
  error = cdev_add(&stopwatch_cdev, stopwatch_dev, 1);
  if (error)
    printk(KERN_NOTICE "stopwatch register error %d\n", error);

  return 0;
}

static int __init stopwatch_init() {
  if ((result = stopwatch_register_cdev()) < 0)
    return result;

  fnd_addr = ioremap(FND_ADDRESS, 0x04);
  init_timer(&timer);
  workqueue = create_workqueue("FND IO");

  printk(KERN_ALERT "Init Module Success\n");
  printk(KERN_ALERT "Device: %s, Major Number: %d\n", DEVICE_DRIVER, stopwatch_major);

  return 0;
}

static void __exit stopwatch_exit() {
  del_timer_sync(&timer);
  iounmap(fnd_addr);
  cdev_del(&stopwatch_cdev);
  unregister_chrdev_region(stopwatch_dev, 1);
  destroy_workqueue(workqueue);

  printk(KERN_ALERT "Remove Module Success\n");
}

module_init(stopwatch_init);
module_exit(stopwatch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("msh0117@sogang.ac.kr");
