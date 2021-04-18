/**
 * Embedded system software, 2021
 *
 * draw_board.c - draw board function implementation
 */
#include "draw_board.h"

/**
 * init_draw_board - initializes @status to the default status of mode 4(Draw board)
 *
 * @status: device status to initialize
 */
void init_draw_board(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->mode_4_dot_matrix, 0x00, 10);
  status->led_val                   = 0x00;
  status->mode_4_switch_cnt         = 0;
  status->mode_4_cursor_x           = 0;
  status->mode_4_cursor_y           = 0;
  status->mode_4_cursor_on          = true;
  status->mode_4_cursor_current_on  = true;
}

void draw_board_update_switch_cnt(struct device_status *status) {
  status->mode_4_switch_cnt = (status->mode_4_switch_cnt+1)%10000;
  status->fnd_val[0]        = status->mode_4_switch_cnt/1000;
  status->fnd_val[1]        = (status->mode_4_switch_cnt%1000)/100;
  status->fnd_val[2]        = (status->mode_4_switch_cnt%100)/10;
  status->fnd_val[3]        = status->mode_4_switch_cnt%10;
}

/**
 * handle_draw_board - handles mode 4(Draw board)
 *
 * @status: current status of the device
 */
void handle_draw_board(struct device_status *status) {
  if (status->switch_val[1] == KEY_PRESS) {
    status->mode_4_cursor_y = (status->mode_4_cursor_y+9)%10;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[3] == KEY_PRESS) {
    status->mode_4_cursor_x = (status->mode_4_cursor_x+6)%7;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[5] == KEY_PRESS) {
    status->mode_4_cursor_x = (status->mode_4_cursor_x+1)%7;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[7] == KEY_PRESS) {
    status->mode_4_cursor_y = (status->mode_4_cursor_y+1)%10;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[4] == KEY_PRESS) {
    status->mode_4_dot_matrix[status->mode_4_cursor_y] |= (1 << (6-status->mode_4_cursor_x));
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[0] == KEY_PRESS) {
    memset(status->mode_4_dot_matrix, 0x00, 10);
    status->mode_4_cursor_x           = 0;
    status->mode_4_cursor_y           = 0;
    status->mode_4_cursor_on          = true;
    status->mode_4_cursor_current_on  = true;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[2] == KEY_PRESS) {
    status->mode_4_cursor_on = !status->mode_4_cursor_on;
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[6] == KEY_PRESS) {
    memset(status->mode_4_dot_matrix, 0x00, 10);
    draw_board_update_switch_cnt(status);
  }

  if (status->switch_val[8] == KEY_PRESS) {
    unsigned int i;
    for (i=0; i<10; i++) {
      status->mode_4_dot_matrix[i] = ~status->mode_4_dot_matrix[i];
    }
    draw_board_update_switch_cnt(status);
  }

  memcpy(status->dot_matrix_val, status->mode_4_dot_matrix, 10);

  if (status->mode_4_cursor_on) {
    if (status->mode_4_cursor_current_on) {
      status->dot_matrix_val[status->mode_4_cursor_y] |= (1 << (6-status->mode_4_cursor_x));
    } else {
      status->dot_matrix_val[status->mode_4_cursor_y] &= ~(1 << (6-status->mode_4_cursor_x));
    }
    status->mode_4_cursor_current_on = !status->mode_4_cursor_current_on;
  }
  usleep(600000);
}
