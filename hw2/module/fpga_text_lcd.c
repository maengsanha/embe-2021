/*
 * Embedded System Software, 2021
 *
 * fpga_text_lcd.c - FPGA Text LCD device utility functions implementation
 */
#include "fpga_text_lcd.h"

/**
 * text_lcd_init - initializes @text_lcd_addr to @STU_NO and @NAME
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_init(unsigned char *text_lcd_addr) {
  unsigned int i;
  for (i=15; 0 <= i; --i) {
    text_lcd_addr[i]    = STU_NO[i];
    text_lcd_addr[i+16] = NAME[i];
  }
}

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_exit(unsigned char *text_lcd_addr) {
  unsigned int i;
  for (i=31; 0 <= i; --i) text_lcd_addr[i] = 0x20;
}