#include <string.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

unsigned char display_addr = 0b0111100;
unsigned char display_buffer[513];

void ssd1306_setup() {
    display_buffer[0] = 0x40;
    sleep_ms(20);

    // --- display init sequence ---
    ssd1306_command(SSD1306_DISPLAYOFF);
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(0x80);
    ssd1306_command(SSD1306_SETMULTIPLEX);
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x0);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);

    ssd1306_clear();
    ssd1306_update();
}

void ssd1306_command(unsigned char cmd) {
    uint8_t buf[2];
    buf[0] = 0x00;
    buf[1] = cmd;
    i2c_write_blocking(i2c_default, display_addr, buf, 2, false);
}

void ssd1306_update() {
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(0xFF);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128 - 1);

    unsigned short byte_count = 512;
    unsigned char *buf_ptr = display_buffer;

    i2c_write_blocking(i2c_default, display_addr, buf_ptr, 513, false);
}

void ssd1306_drawPixel(unsigned char px, unsigned char py, unsigned char on) {
    if ((px < 0) || (px >= 128) || (py < 0) || (py >= 32)) {
        return;
    }

    if (on == 1) {
        display_buffer[1 + px + (py / 8) * 128] |= (1 << (py & 7));
    } else {
        display_buffer[1 + px + (py / 8) * 128] &= ~(1 << (py & 7));
    }
}

void ssd1306_clear() {
    memset(display_buffer, 0, 512);
    display_buffer[0] = 0x40;
}
