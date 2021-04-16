#ifndef _TEXT_EDITOR
#define _TEXT_EDITOR

#include "device_status.h"

#define TEXT_EDITOR_MODE 2

/**
 * text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
extern inline void text_editor(struct device_status *status);

#endif