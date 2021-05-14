/*
 * Embedded System Software, 2021
 *
 * device_status.h - FPGA device status definition
 */
#ifndef _DEVICE_STATUS_H
#define _DEVICE_STATUS_H

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

#define VOL_PLUS          0x73
#define VOL_MINUS         0x72
#define BACK              0x9E
#define KEY_RELEASE       0x00
#define KEY_PRESS         0x01
#define CLOCK_MODE        0x00
#define COUNTER_MODE      0x01
#define TEXT_EDITOR_MODE  0x02
#define DRAW_BOARD_MODE   0x03

/**
 * struct device_status - board status
 *
 * @mode:                     current mode of the device (Clock / Counter / Text editor / Draw board)
 * @readkey_val:              current value of READKEY
 * @switch_val:               current value of Switches
 * @fnd_val:                  current value of FND
 * @text_lcd_val:             current value of Text LCD
 * @dot_matrix_val:           current value of Dot Matrix
 * @led_val:                  current value of LEDs
 * 
 * @mode_1_on_change:         whether clock is changable or not
 * @mode_1_hour:              hour value of clock
 * @mode_1_min:               minute value of clock
 *
 * @mode_2_number:            current number of counter (10 -> 8 -> 4 -> 2)
 * @mode_2_val:               substantial value of counter
 *
 * @mode_3_mode:              current mode of Text editor (Alphabet or Number)
 * @mode_3_switch_cnt:        number that switch pressed
 * @mode_3_last_pressed:      number of last pressed switch
 * @mode_3_write_pos:         current position to write on Text LCD
 *
 * @mode_4_switch_cnt:        number that switch pressed
 * @mode_4_cursor_x:          X coordinate of cursor
 * @mode_4_cursor_y:          Y coordinate of cursor
 * @mode_4_cursor_on:         whether show cursor on Dot matrix or not
 * @mode_4_cursor_current_on: whether cursor is now shown on Dot matrix or not
 * @mode_4_dot_matrix         current value of picture
 */
struct device_status {
  unsigned int        mode;                 /* 0 ~ 3 */
  struct input_event  readkey_val[64];
  unsigned char       switch_val[9];
  unsigned char       fnd_val[4];
  unsigned char       text_lcd_val[32];
  unsigned char       dot_matrix_val[10];
  unsigned long       led_val;
  bool                mode_1_on_change;
  unsigned int        mode_1_hour;
  unsigned int        mode_1_min;
  unsigned int        mode_2_number;        /* 2 or 4 or 8 or 10 */
  unsigned int        mode_2_val;
  unsigned int        mode_3_mode;          /* 0 or 1 */
  unsigned int        mode_3_switch_cnt;    /* 0 ~ 9999 */
  unsigned int        mode_3_last_pressed;  /* 0 ~ 8 */
  unsigned int        mode_3_write_pos;     /* 0 ~ 31 */
  unsigned int        mode_4_switch_cnt;    /* 0 ~ 9999 */
  unsigned int        mode_4_cursor_x;      /* 0 ~ 6 */
  unsigned int        mode_4_cursor_y;      /* 0 ~ 9 */
  bool                mode_4_cursor_on;
  bool                mode_4_cursor_current_on;
  unsigned char       mode_4_dot_matrix[10];
};

/**
 * init_status - initializes @status to the default status
 *
 * @status: device status to initialize
 */
void init_status(struct device_status *status);

#endif
