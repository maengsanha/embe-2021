/**
 * struct device_status - board status
 * 
 * @mode:       current mode of device
 * @led:        current value of LEDs
 * @fnd:        current value of FND
 * @swtch:      current value of Switches
 * @vol:        current value of Volume
 * @text_lcd:   current value of Text LCD
 * @dot_matrix: current value of Dot Matrix
 */
struct device_status {
  unsigned int  mode;         /* 0 ~ 3 */
  unsigned int  led;          /* 0x00 ~ 0xFF */
  unsigned int  fnd;          /* 0x0 ~ 0xF */
  unsigned int  swtch;        /* 000000000 ~ 111111111 */
  unsigned int  vol;          /* 0 ~ ? */
  char          **text_lcd;   /* char[2][16] */
  unsigned int  **dot_matrix; /* int[10][8] */
};