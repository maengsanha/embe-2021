#include "draw_board.h"

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