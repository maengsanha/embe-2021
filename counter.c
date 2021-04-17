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
  status->led_val = 0x40;
}

/**
 * handle_counter - handles mode 2(Counter)
 *
 * @status: current status of the device
 */
void handle_counter(struct device_status *status) {
  printf("counter...\n");
}