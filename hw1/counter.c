/*
 * Embedded System Software, 2021
 *
 * counter.c - counter function implementation
 */
#include "counter.h"

/**
 * init_counter - initializes @status to the default value of mode 2(Counter)
 *
 * @status: device status to initialize
 */
void init_counter(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->dot_matrix_val, 0x00, 10);
  status->led_val       = 0x40;
  status->mode_2_number = 10;
  status->mode_2_val    = 0;
}

/**
 * handle_counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
void handle_counter(struct device_status *status) {
  if (status->switch_val[0] == KEY_PRESS) counter_switch_1(status);
  if (status->switch_val[1] == KEY_PRESS) counter_switch_2(status);
  if (status->switch_val[2] == KEY_PRESS) counter_switch_3(status);
  if (status->switch_val[3] == KEY_PRESS) counter_switch_4(status);
  counter_update_fnd(status);
}

void counter_switch_1(struct device_status *status) {
  switch (status->mode_2_number) {
  case 10:
    status->mode_2_number = 8;
    status->led_val       = 0x20;
    break;
  case 8:
    status->mode_2_number = 4;
    status->led_val       = 0x10;
    break;
  case 4:
    status->mode_2_number = 2;
    status->led_val       = 0x80;
    break;
  case 2:
    status->mode_2_number = 10;
    status->led_val       = 0x40;
    break;
  }
}

void counter_switch_2(struct device_status *status) {
  switch (status->mode_2_number) {
  case 10:
    status->mode_2_val += 100;
    break;
  case 8:
    status->mode_2_val += 0x40;
    break;
  case 4:
    status->mode_2_val += 0x10;
    break;
  case 2:
    status->mode_2_val += 0x04;
    break;
  }
}

void counter_switch_3(struct device_status *status) {
  switch (status->mode_2_number) {
  case 10:
    status->mode_2_val += 10;
    break;
  case 8:
    status->mode_2_val += 0x08;
    break;
  case 4:
    status->mode_2_val += 0x04;
    break;
  case 2:
    status->mode_2_val += 0x02;
    break;
  }
}

void counter_switch_4(struct device_status *status) { status->mode_2_val++; }

void counter_update_fnd(struct device_status *status) {
  unsigned int val = status->mode_2_val;

  status->fnd_val[0] = 0;
  val               %= status->mode_2_number*status->mode_2_number*status->mode_2_number;

  status->fnd_val[1] = val / (status->mode_2_number*status->mode_2_number);
  val               %= status->mode_2_number*status->mode_2_number;

  status->fnd_val[2] = val / status->mode_2_number;
  val               %= status->mode_2_number;

  status->fnd_val[3] = val;
}
