#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_BUS        spi0
#define GPIO_MISO      16
#define GPIO_CS        17
#define GPIO_SCK       18
#define GPIO_MOSI      19

#define DAC_CHANNEL_A  0
#define DAC_CHANNEL_B  1

#define LOOP_RATE_HZ      1000
#define LOOP_PERIOD_US    1000

static inline void dac_cs_assert(uint pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(pin, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void dac_cs_release(uint pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(pin, 1);
    asm volatile("nop \n nop \n nop");
}

static void dac_send(uint8_t chan, uint16_t code) {
    uint16_t word = (1u << 13) | (1u << 12);
    if (chan == DAC_CHANNEL_B) word |= (1u << 15);
    word |= (code & 0x03FFu) << 2;

    uint8_t frame[2] = { (uint8_t)(word >> 8), (uint8_t)(word & 0xFF) };

    dac_cs_assert(GPIO_CS);
    spi_write_blocking(SPI_BUS, frame, 2);
    dac_cs_release(GPIO_CS);
}

static void setup_spi(void) {
    spi_init(SPI_BUS, 1000 * 1000);
    spi_set_format(SPI_BUS, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(GPIO_MISO, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(GPIO_MOSI, GPIO_FUNC_SPI);

    gpio_init(GPIO_CS);
    gpio_set_dir(GPIO_CS, GPIO_OUT);
    gpio_put(GPIO_CS, 1);
}

static uint16_t sine_sample(float phase) {
    return (uint16_t)((sinf(phase) + 1.0f) * 0.5f * 1023.0f);
}

static uint16_t triangle_sample(int step, int period) {
    int half = period / 2;
    int level = (step < half) ? step : (period - step);
    return (uint16_t)((float)level / (float)half * 1023.0f);
}

int main() {
    stdio_init_all();
    setup_spi();

    const float phase_step = 2.0f * (float)M_PI * 2.0f / (float)LOOP_RATE_HZ;
    const int tri_period = LOOP_RATE_HZ;

    float phase_a = 0.0f;
    int step_b = 0;

    absolute_time_t wake_time = get_absolute_time();

    while (true) {
        sleep_until(wake_time);
        wake_time = delayed_by_us(wake_time, LOOP_PERIOD_US);

        dac_send(DAC_CHANNEL_A, sine_sample(phase_a));
        dac_send(DAC_CHANNEL_B, triangle_sample(step_b, tri_period));

        phase_a += phase_step;
        if (phase_a >= 2.0f * (float)M_PI) phase_a -= 2.0f * (float)M_PI;

        step_b = (step_b + 1) % tri_period;
    }

    return 0;
}
