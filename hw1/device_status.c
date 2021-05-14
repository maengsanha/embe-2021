/*
 * Embedded System Software, 2021
 *
 * device_status.c - FPGA device status implementation
 */
#include "device_status.h"

/**
 * init_status - initializes @status to the default status
 *
 * @status: device status to initialize
 */
void init_status(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->dot_matrix_val, 0x00, 10);
  status->led_val = 0x00;
}
