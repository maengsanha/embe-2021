#include "text_editor.h"

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