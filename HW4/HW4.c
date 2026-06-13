#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

#define SDA_PIN 20
#define SCL_PIN 21

static void setup_peripherals(void);
void plot_char(int px, int py, char ch);
void plot_string(int px, int py, char *text);

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) return -1;

    setup_peripherals();

    unsigned int last_time_us = to_us_since_boot(get_absolute_time());
    unsigned int blink_time_us = last_time_us;
    unsigned int move_time_us = last_time_us;
    int led_state = 0;
    int hello_visible = 1;
    int hello_x = 0, hello_y = 12;
    float frame_rate = 0.0f;

    srand(last_time_us);

    //main loop
    for (;;) {
        unsigned int now_us = to_us_since_boot(get_absolute_time());
        printf("hello");

        if (now_us - blink_time_us >= 500000) {
            blink_time_us += 500000;
            led_state = !led_state;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
        }

        unsigned int delta_us = now_us - last_time_us;
        if (delta_us > 0) frame_rate = 1000000.0f / delta_us;
        last_time_us = now_us;

        uint16_t raw_adc = adc_read();
        float volts = raw_adc * 3.3f / 4096.0f;

        ssd1306_clear();

        char line_buf[32];
        sprintf(line_buf, "ADC0 = %.3f V", volts);
        plot_string(0, 0, line_buf);

        sprintf(line_buf, "FPS: %.1f", frame_rate);
        plot_string(0, 24, line_buf);

        ssd1306_update();
    }
}

static void setup_peripherals(void) {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    ssd1306_setup();

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
}

void plot_char(int px, int py, char ch) {
    if (ch < 0x20 || ch > 0x7f) return;

    int glyph_index = ch - 0x20;
    int col_idx = 0;
    while (col_idx < 5) {
        unsigned char col_bits = FONT_TABLE[glyph_index * 5 + col_idx];
        for (int row_idx = 0; row_idx < 8; row_idx++) {
            ssd1306_drawPixel(px + col_idx, py + row_idx, (col_bits >> row_idx) & 1);
        }
        col_idx++;
    }
}

void plot_string(int px, int py, char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        plot_char(px + i * 6, py, text[i]);
    }
}
