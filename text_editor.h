/**
 * Embedded system software, 2021
 *
 * text_editor.h - text editor function defition
 */
#ifndef _TEXT_EDITOR_H
#define _TEXT_EDITOR_H

#include "device_status.h"

#define ALPHABET_MODE 0
#define NUMBER_MODE   1

static const unsigned char DOT_MATRIX_1[10] = {0x0c, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3f, 0x3f};
static const unsigned char DOT_MATRIX_A[10] = {0x1c, 0x36, 0x63, 0x63, 0x63, 0x7f, 0x7f, 0x63, 0x63, 0x63};

static const unsigned char ALPHABETS[9][3] = {
  {0x2e, 0x51, 0x5A},
  {0x41, 0x42, 0x43},
  {0x44, 0x45, 0x46},
  {0x47, 0x48, 0x49},
  {0x4A, 0x4B, 0x4C},
  {0x4D, 0x4E, 0x4F},
  {0x50, 0x52, 0x53},
  {0x54, 0x55, 0x56},
  {0x57, 0x58, 0x59},
};

/**
 * init_text_editor - initializes @status to the default status of mode 3(Text editor)
 *
 * @status: device status to initialize
 */
void init_text_editor(struct device_status *status);

/**
 * handle_text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
void handle_text_editor(struct device_status *status);

#endif
