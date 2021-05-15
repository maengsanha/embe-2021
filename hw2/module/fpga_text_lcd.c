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
  memcpy(text_lcd_addr, STU_NO, 16);
  memcpy(&text_lcd_addr[16], NAME, 16);
}

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_exit(unsigned char *text_lcd_addr) { memset(text_lcd_addr, 0x20, 32); }