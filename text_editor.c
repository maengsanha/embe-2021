#include "text_editor.h"

/**
 * init_text_editor - initializes @status to the default status of mode 3(Text editor)
 *
 * @status: device status to initialize
 */
void init_text_editor(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  memset(status->dot_matrix_val, 0x00, 10);
  status->led_val = 0x00;
}

/**
 * handle_text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
void handle_text_editor(struct device_status *status) {
  do {
    if (status->readkey_val[0].code == BACK) break;
    printf("text editor...\n");
    sleep(1);
  } while (status->readkey_val[0].code == TEXT_EDITOR_MODE);
}