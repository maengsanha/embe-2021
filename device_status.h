#ifndef _DEVICE_STATUS
#define _DEVICE_STATUS

#include <string.h>

/**
 * struct device_status - board status
 * 
 * @mode:           current mode of device
 * @fnd_fd:         file descriptor of FND
 * @text_lcd_fd:    file descriptor of Text LCD
 * @switch_fd:      file descriptor of Switches
 * @dot_matrix_fd:  file descriptor of Dot Matrix
 * @led_addr:       address of LEDs
 * @fnd_val:        current value of FND
 * @text_lcd_val:   current value of Text LCD
 * @switch_val:     current value of Switches
 * @dot_matrix_val: current value of Dot Matrix
 * @led_val:        current value of LEDs
 */
struct device_status {
  unsigned int  mode; /* 0 ~ 3 */
  int           fnd_fd;
  int           text_lcd_fd;
  int           switch_fd;
  int           dot_matrix_fd;
  unsigned long *led_addr;
  unsigned char fnd_val[4];
  unsigned char text_lcd_val[32];
  unsigned char switch_val[9];
  unsigned char dot_matrix_val[10];
  unsigned long led_val;
  // volume
};

extern inline void set_fnd_value(struct device_status *status, const unsigned char val[4]) { memcpy(status->fnd_val, val, sizeof(status->fnd_val)); }

extern inline void set_text_lcd_value(struct device_status *status, const unsigned char val[32]) { memcpy(status->text_lcd_val, val, sizeof(status->text_lcd_val)); }

extern inline void set_switch_value(struct device_status *status, const unsigned char val[9]) { memcpy(status->switch_val, val, sizeof(status->switch_val)); }

extern inline void set_dot_matrix_value(struct device_status *status, const unsigned char val[10]) { memcpy(status->dot_matrix_val, val, sizeof(status->dot_matrix_val)); }

extern inline void set_led_value(struct device_status *status, const unsigned long val) { status->led_val = val; }

/**
 * init_device - initializes @status
 *
 * @status:         device status to be initialized
 * @fnd_fd:         file descriptor of FND
 * @text_lcd_fd:    file descriptor of Text LCD
 * @switch_fd:      file descriptor of Switches
 * @dot_matrix_fd:  file descriptor of Dot Matrix
 * @led_addr:       address of LEDs
 */
extern inline void init_device(struct device_status *status, const int fnd_fd,
  const int text_lcd_fd, const int switch_fd,
  const int dot_matrix_fd, const unsigned long *led_addr) {
  status->fnd_fd        = fnd_fd;
  status->text_lcd_fd   = text_lcd_fd;
  status->switch_fd     = switch_fd;
  status->dot_matrix_fd = dot_matrix_fd;
  status->led_addr      = led_addr;
  status->mode          = 0;
  set_fnd_value(status, "0000");
  set_text_lcd_value(status, "");
  set_switch_value(status, "");
  set_dot_matrix_value(status, "");
  set_led_value(status, 0x00);
}

#endif