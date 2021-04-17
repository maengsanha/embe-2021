#include "clock.h"

/**
 * init_clock - initializes @status to the default value of mode 1(Clock)
 *
 * @status: device status to initialize
 */
void init_clock(struct device_status *status) {
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->dot_matrix_val, 0x00, 10);
  time_t rawtime;
  time(&rawtime);
  struct tm *now            = localtime(&rawtime);
  status->mode_1_on_change  = false;
  status->mode_1_hour       = now->tm_hour;
  status->mode_1_min        = now->tm_min;
  status->fnd_val[0]        = status->mode_1_hour/10;
  status->fnd_val[1]        = status->mode_1_hour%10;
  status->fnd_val[2]        = status->mode_1_min/10;
  status->fnd_val[3]        = status->mode_1_min%10;
  status->led_val           = 0x80;
}

/**
 * handle_clock - handles mode 1(Clock)
 *
 * @status: current status of the device
 */
void handle_clock(struct device_status *status) {
  // if SW[2] pressed, reset
  if (status->switch_val[1] == KEY_PRESS)
    init_clock(status);

  // if SW[1] pressed, start or save
  if (status->switch_val[0] == KEY_PRESS) {
    if (status->mode_1_on_change) {
      status->led_val           = 0x80;
      status->fnd_val[0]        = status->mode_1_hour/10;
      status->fnd_val[1]        = status->mode_1_hour%10;
      status->fnd_val[2]        = status->mode_1_min/10;
      status->fnd_val[3]        = status->mode_1_min%10;
      status->mode_1_on_change  = false;
    } else {
      status->mode_1_on_change  = true;
    }
  }

  // if SW[3] pressed, increase hour
  if (status->switch_val[2] == KEY_PRESS && status->mode_1_on_change) {
    status->mode_1_hour = (status->mode_1_hour+1)%24;
  }

  // if SW[4] pressed, increase minute
  if (status->switch_val[3] == KEY_PRESS && status->mode_1_on_change) {
    status->mode_1_hour = (status->mode_1_hour+(status->mode_1_min+1)/60)%24;
    status->mode_1_min  = (status->mode_1_min+1)%60;
  }

  if (status->mode_1_on_change) status->led_val = status->led_val == 0x20 ? 0x10 : 0x20;
  usleep(600000);
}