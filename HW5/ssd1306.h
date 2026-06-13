#ifndef SSD1306_H__
#define SSD1306_H__

#define OLED_CMD_MEMORY_MODE       0x20
#define OLED_CMD_COLUMN_ADDR       0x21
#define OLED_CMD_PAGE_ADDR         0x22
#define OLED_CMD_SET_CONTRAST      0x81
#define OLED_CMD_CHARGE_PUMP       0x8D
#define OLED_CMD_SEGREMAP          0xA0
#define OLED_CMD_DISPLAY_ALL_ON    0xA4
#define OLED_CMD_NORMAL_DISPLAY    0xA6
#define OLED_CMD_INVERT_DISPLAY    0xA7
#define OLED_CMD_SET_MULTIPLEX     0xA8
#define OLED_CMD_DISPLAY_OFF       0xAE
#define OLED_CMD_DISPLAY_ON        0xAF
#define OLED_CMD_COM_SCAN_DEC      0xC8
#define OLED_CMD_SET_DISPLAY_OFFSET 0xD3
#define OLED_CMD_SET_CLOCK_DIV     0xD5
#define OLED_CMD_SET_PRECHARGE     0xD9
#define OLED_CMD_SET_COM_PINS      0xDA
#define OLED_CMD_SET_VCOM_DETECT   0xDB
#define OLED_CMD_SET_START_LINE    0x40
#define OLED_CMD_DEACTIVATE_SCROLL 0x2E

void oled_init(void);
void oled_flush(void);
void oled_clear(void);
void oled_set_pixel(unsigned char x, unsigned char y, unsigned char color);
void oled_write_command(unsigned char cmd);

#endif
