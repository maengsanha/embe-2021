/**
 * Embedded system software, 2021
 *
 * text_editor.c - text editor function implementation
 */
#include "text_editor.h"

void text_editor_set_dot_matrix(struct device_status *status) {
  status->mode_3_mode == ALPHABET_MODE ? memcpy(status->dot_matrix_val, DOT_MATRIX_A, 10)
                                       : memcpy(status->dot_matrix_val, DOT_MATRIX_1, 10);
}

/**
 * init_text_editor - initializes @status to the default status of mode 3(Text editor)
 *
 * @status: device status to initialize
 */
void init_text_editor(struct device_status *status) {
  memset(status->fnd_val, 0x00, 4);
  memset(status->text_lcd_val, 0x20, 32);
  status->led_val             = 0x00;
  status->mode_3_switch_cnt   = 0x00;
  status->mode_3_last_pressed = 0x09;
  status->mode_3_write_pos    = 0x00;
  status->mode_3_mode         = ALPHABET_MODE;
  text_editor_set_dot_matrix(status);
}

void text_editor_update_switch_cnt(struct device_status *status) {
  status->mode_3_switch_cnt = (status->mode_3_switch_cnt+1)%10000;
  status->fnd_val[0]        = status->mode_3_switch_cnt/1000;
  status->fnd_val[1]        = (status->mode_3_switch_cnt%1000)/100;
  status->fnd_val[2]        = (status->mode_3_switch_cnt%100)/10;
  status->fnd_val[3]        = status->mode_3_switch_cnt%10;
}

void text_editor_switch23(struct device_status *status) {
  status->mode_3_mode = ALPHABET_MODE;
  text_editor_set_dot_matrix(status);
  memset(status->text_lcd_val, 0x20, 32);
  status->mode_3_write_pos    = 0;
  status->mode_3_last_pressed = 9;
}

void text_editor_switch56(struct device_status *status) {
  status->mode_3_mode = status->mode_3_mode == ALPHABET_MODE ? NUMBER_MODE : ALPHABET_MODE;
  text_editor_set_dot_matrix(status);
  status->mode_3_last_pressed = 9;
}

void text_editor_switch89(struct device_status *status) {
  if (status->mode_3_write_pos == 31) {
    memcpy(status->text_lcd_val, &status->text_lcd_val[1], 31);
    status->text_lcd_val[status->mode_3_write_pos] = 0x20;
  } else {
    status->text_lcd_val[status->mode_3_write_pos] = 0x20;
    status->mode_3_write_pos++;
  }
  status->mode_3_last_pressed = 9;
}

/**
 * handle_text_editor - handles mode 3(Text editor)
 *
 * @status: current status of the device
 */
void handle_text_editor(struct device_status *status) {
  if (status->switch_val[1] == KEY_PRESS && status->switch_val[2] == KEY_PRESS) {
    text_editor_update_switch_cnt(status);
    text_editor_switch23(status);
    return;
  }

  if (status->switch_val[4] == KEY_PRESS && status->switch_val[5] == KEY_PRESS) {
    text_editor_update_switch_cnt(status);
    text_editor_switch56(status);
    return;
  }

  if (status->switch_val[7] == KEY_PRESS && status->switch_val[8] == KEY_PRESS) {
    text_editor_update_switch_cnt(status);
    text_editor_switch89(status);
    return;
  }

  unsigned int i;
  for (i=0; i<9; ++i) {
    if (status->switch_val[i] == KEY_PRESS) {
      text_editor_update_switch_cnt(status);
      if (status->mode_3_mode == NUMBER_MODE) {
        if (status->mode_3_write_pos == 31) {
          memcpy(status->text_lcd_val, &status->text_lcd_val[1], 31);
          status->text_lcd_val[status->mode_3_write_pos] = i+49;
        } else {
          status->text_lcd_val[status->mode_3_write_pos] = i+49;
          status->mode_3_write_pos++;
        }
      } else {
        if (status->mode_3_last_pressed == i) {
          if (status->text_lcd_val[status->mode_3_write_pos-1] == ALPHABETS[i][0]) {
            status->text_lcd_val[status->mode_3_write_pos-1] = ALPHABETS[i][1];
          } else if (status->text_lcd_val[status->mode_3_write_pos-1] == ALPHABETS[i][1]) {
            status->text_lcd_val[status->mode_3_write_pos-1] = ALPHABETS[i][2];
          } else if (status->text_lcd_val[status->mode_3_write_pos-1] == ALPHABETS[i][2]) {
            status->text_lcd_val[status->mode_3_write_pos-1] = ALPHABETS[i][0];
          }
        } else {
          if (status->mode_3_write_pos == 31) {
            memcpy(status->text_lcd_val, &status->text_lcd_val[1], 31);
            status->text_lcd_val[status->mode_3_write_pos] = ALPHABETS[i][0];
          } else {
            status->text_lcd_val[status->mode_3_write_pos] = ALPHABETS[i][0];
            status->mode_3_write_pos++;
          }
          status->mode_3_last_pressed = i;
        }
      }
      break;
    }
  }
}