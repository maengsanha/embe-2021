#ifndef _DEVICE_STATUS
#define _DEVICE_STATUS

#include <string.h>
#include <linux/input.h>

/**
 * struct device_status - board status
 *
 * @mode:           current mode of the device (Clock / Counter / Text editor / Draw board)
 * @readkey_val:    current value of READKEY
 * @switch_val:     current value of Switches
 * @fnd_val:        current value of FND
 * @text_lcd_val:   current value of Text LCD
 * @dot_matrix_val: current value of Dot Matrix
 * @led_val:        current value of LEDs
 */
struct device_status {
  unsigned int        mode; /* 0 ~ 3 */
  struct input_event  readkey_val[64];
  unsigned char       switch_val[9];
  unsigned char       fnd_val[4];
  unsigned char       text_lcd_val[32];
  unsigned char       dot_matrix_val[10];
  unsigned long       led_val;
};

extern inline void set_fnd_value(struct device_status *status, const unsigned char val[4]);
extern inline void set_text_lcd_value(struct device_status *status, const unsigned char val[32]);
extern inline void set_dot_matrix_value(struct device_status *status, const unsigned char val[10]);
extern inline void set_led_value(struct device_status *status, const unsigned long val);

/**
 * init_status - initializes @status
 *
 * @status: device status to initialize
 */
extern inline void init_status(struct device_status *status, unsigned int mode);

#endif
