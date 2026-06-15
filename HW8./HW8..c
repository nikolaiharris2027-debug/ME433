#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_PORT     spi0
#define PIN_MISO     16
#define PIN_CS_DAC   17
#define PIN_SCK      18
#define PIN_MOSI     19
#define PIN_CS_RAM   20

#define RAM_CMD_WRITE   0x02
#define RAM_CMD_READ    0x03
#define RAM_CMD_STATUS  0x01

#define RAM_MODE_SEQ    0x40

#define NUM_SAMPLES  1000

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop");
}

void spi_ram_init(void) {
    uint8_t cmd[2] = { RAM_CMD_STATUS, RAM_MODE_SEQ };
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, cmd, 2);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_write(uint16_t addr, uint8_t *data, uint16_t len) {
    uint8_t header[3] = { RAM_CMD_WRITE, (addr >> 8) & 0xFF, addr & 0xFF };
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_read(uint16_t addr, uint8_t *data, uint16_t len) {
    uint8_t header[3] = { RAM_CMD_READ, (addr >> 8) & 0xFF, addr & 0xFF };
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_read_blocking(SPI_PORT, 0x00, data, len);
    cs_deselect(PIN_CS_RAM);
}

static uint16_t float_to_dac_cmd(float voltage) {
    if (voltage < 0.0f)  voltage = 0.0f;
    if (voltage > 3.3f)  voltage = 3.3f;

    uint16_t val = (uint16_t)(voltage / 3.3f * 1023.0f);
    return (1u << 13) | (1u << 12) | ((val & 0x3FFu) << 2);
}

int main(void) {
    stdio_init_all();

    spi_init(SPI_PORT, 1000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS_DAC);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);

    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);

    spi_ram_init();

    for (int i = 0; i < NUM_SAMPLES; i++) {
        float voltage = (sinf(2.0f * (float)M_PI * i / NUM_SAMPLES) + 1.0f)
                        * 0.5f * 3.3f;

        uint16_t cmd = float_to_dac_cmd(voltage);
        uint8_t bytes[2] = { (uint8_t)(cmd >> 8), (uint8_t)(cmd & 0xFF) };

        spi_ram_write(i * 2, bytes, 2);
    }

    while (true) {
        for (int i = 0; i < NUM_SAMPLES; i++) {
            uint8_t bytes[2];
            spi_ram_read(i * 2, bytes, 2);

            cs_select(PIN_CS_DAC);
            spi_write_blocking(SPI_PORT, bytes, 2);
            cs_deselect(PIN_CS_DAC);

            sleep_ms(1);
        }
    }

    return 0;
}
