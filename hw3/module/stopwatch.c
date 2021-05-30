/*
 * Embedded System Software, 2021
 *
 * stopwatch.c - stop watch device drvier
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <asm/irq.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>

#define FND_ADDRESS   0x08000004
#define DEVICE_DRIVER "/dev/stopwatch"

static int               stopwatch_major = 242;
static int               stopwatch_minor = 0;
static int               result;
static unsigned char     *fnd_addr;
static dev_t             stopwatch_dev;
static struct cdev       stopwatch_cdev;
static struct timer_list timer;

static int done    = 0;
static int count   = 0;
static int fnd_val = 0;

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
 * fnd_init - initialize @fnd_addr
 */
static inline void fnd_init() { fnd_write(0); }

///////////////////////////////////////////////////////////// Stopwatch Device /////////////////////////////////////////////////////////////

irqreturn_t stopwatch_handler1(int irq, void *dev_id, struct pt_regs *reg) {
  fnd_write(++fnd_val);
  printk("HOME\n");

  if (5 < fnd_val) {
    fnd_init();
    done = 1;
    __wake_up(&wq_head, 1, 1, NULL);
    printk("wake up\n");
  }

  return IRQ_HANDLED;
}

irqreturn_t stopwatch_handler2(int irq, void *dev_id, struct pt_regs *reg) {
  fnd_val = 0;
  fnd_init();
  printk("BACK\n");
  return IRQ_HANDLED;
}

irqreturn_t stopwatch_handler3(int irq, void *dev_id, struct pt_regs *reg) {
  printk("VOL+\n");
  return IRQ_HANDLED;
}

irqreturn_t stopwatch_handler4(int irq, void *dev_id, struct pt_regs *reg) {
  printk("VOL-\n");
  return IRQ_HANDLED;
}

/**
 * stopwatch_open - stopwatch opening event handler
 *
 * @inodp: not used
 * @filp:  not used
 */
static int stopwatch_open(struct inode *inodp, struct file *filp) {
  int irq, ret;

  printk(KERN_ALERT "Open Module\n");
  
  gpio_direction_input(IMX_GPIO_NR(1, 11));
  irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
  ret = request_irq(irq, stopwatch_handler1, IRQF_TRIGGER_FALLING, "home", 0);

  gpio_direction_input(IMX_GPIO_NR(1, 12));
  irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
  ret = request_irq(irq, stopwatch_handler2, IRQF_TRIGGER_FALLING, "back", 0);

  gpio_direction_input(IMX_GPIO_NR(2, 15));
  irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
  ret = request_irq(irq, stopwatch_handler3, IRQF_TRIGGER_FALLING, "volup", 0);

  gpio_direction_input(IMX_GPIO_NR(5, 14));
  irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
  ret = request_irq(irq, stopwatch_handler4, IRQF_TRIGGER_FALLING, "voldown", 0);

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

  printk(KERN_ALERT "Init Module Success\n");
  printk(KERN_ALERT "Device: %s, Major Number: %d\n", DEVICE_DRIVER, stopwatch_major);

  return 0;
}

static void __exit stopwatch_exit() {
  cdev_del(&stopwatch_cdev);
  unregister_chrdev_region(stopwatch_dev, 1);
  iounmap(fnd_addr);

  printk(KERN_ALERT "Remove Module Success\n");
}

module_init(stopwatch_init);
module_exit(stopwatch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("msh0117@sogang.ac.kr");
