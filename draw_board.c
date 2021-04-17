#include "draw_board.h"

/**
 * init_draw_board - initializes @status to the default status of mode 4(Draw board)
 *
 * @status: device status to initialize
 */
void init_draw_board(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->dot_matrix_val, 0x00, 10);
  status->led_val = 0x00;
}

/**
 * handle_draw_board - handles mode 4(Draw board)
 *
 * @status: current status of the device
 */
void handle_draw_board(struct device_status *status) {
  do {
    if (status->readkey_val[0].code == BACK) break;
    printf("draw board...\n");
    sleep(1);
  } while (status->readkey_val[0].code == DRAW_BOARD_MODE);
}