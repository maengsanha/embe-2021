/**
 * Embedded system software, 2021
 *
 * draw_board.h - draw board function defition
 */
#ifndef _DRAW_BOARD_H
#define _DRAW_BOARD_H

#include "device_status.h"

/**
 * init_draw_board - initializes @status to the default status of mode 4(Draw board)
 *
 * @status: device status to initialize
 */
void init_draw_board(struct device_status *status);

/**
 * handle_draw_board - handles mode 4(Draw board)
 *
 * @status: current status of the device
 */
void handle_draw_board(struct device_status *status);

#endif
