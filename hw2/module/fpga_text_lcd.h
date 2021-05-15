/*
 * Embedded System Software, 2021
 *
 * fpga_text_lcd.h - FPGA Text LCD device utility functions definition
 */
#include <string.h>

#include "args.h"

const char STU_NO[16] = "120210207";
const char NAME[16] = "Sanha Maeng";

/**
 * text_lcd_init - initializes @text_lcd_addr to @STU_NO and @NAME
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_init(unsigned char *text_lcd_addr);

/**
 * text_lcd_exit - initialzies @text_lcd_addr to zero value
 *
 * @text_lcd_addr: the address of Text LCD device
 */
inline void text_lcd_exit(unsigned char *text_lcd_addr);