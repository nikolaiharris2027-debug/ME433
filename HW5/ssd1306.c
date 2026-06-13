#include <string.h>
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define OLED_WIDTH  128
#define OLED_HEIGHT 32
#define OLED_BUFFER_SIZE 513

unsigned char OLED_I2C_ADDRESS = 0b0111100;
unsigned char oled_frame_buffer[OLED_BUFFER_SIZE];

static const unsigned char OLED_INIT_SEQUENCE[] = {
    OLED_CMD_DISPLAY_OFF,
    OLED_CMD_SET_CLOCK_DIV, 0x80,
    OLED_CMD_SET_MULTIPLEX, 0x1F,
    OLED_CMD_SET_DISPLAY_OFFSET, 0x00,
    OLED_CMD_SET_START_LINE,
    OLED_CMD_CHARGE_PUMP, 0x14,
    OLED_CMD_MEMORY_MODE, 0x00,
    OLED_CMD_SEGREMAP | 0x1,
    OLED_CMD_COM_SCAN_DEC,
    OLED_CMD_SET_COM_PINS, 0x02,
    OLED_CMD_SET_CONTRAST, 0x8F,
    OLED_CMD_SET_PRECHARGE, 0xF1,
    OLED_CMD_SET_VCOM_DETECT, 0x40,
    OLED_CMD_DISPLAY_ON,
};

void oled_write_command(unsigned char cmd) {
    uint8_t packet[2] = {0x00, cmd};
    i2c_write_blocking(i2c_default, OLED_I2C_ADDRESS, packet, 2, false);
}

void oled_init(void) {
    oled_frame_buffer[0] = 0x40;
    sleep_ms(20);

    for (size_t i = 0; i < sizeof(OLED_INIT_SEQUENCE); i++) {
        oled_write_command(OLED_INIT_SEQUENCE[i]);
    }

    oled_clear();
    oled_flush();
}

void oled_flush(void) {
    oled_write_command(OLED_CMD_PAGE_ADDR);
    oled_write_command(0);
    oled_write_command(0xFF);
    oled_write_command(OLED_CMD_COLUMN_ADDR);
    oled_write_command(0);
    oled_write_command(OLED_WIDTH - 1);

    i2c_write_blocking(i2c_default, OLED_I2C_ADDRESS, oled_frame_buffer, OLED_BUFFER_SIZE, false);
}

void oled_set_pixel(unsigned char x, unsigned char y, unsigned char color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) {
        return;
    }

    size_t byte_index = 1 + x + (y / 8) * OLED_WIDTH;
    if (color == 1) {
        oled_frame_buffer[byte_index] |= (1 << (y & 7));
    } else {
        oled_frame_buffer[byte_index] &= ~(1 << (y & 7));
    }
}

void oled_clear(void) {
    memset(oled_frame_buffer, 0, OLED_BUFFER_SIZE - 1);
    oled_frame_buffer[0] = 0x40;
}
