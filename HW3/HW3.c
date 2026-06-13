#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

enum {
    EXPANDER_ADDR = 0x20,

    REG_IODIR   = 0x00,
    REG_IPOL    = 0x01,
    REG_GPINTEN = 0x02,
    REG_DEFVAL  = 0x03,
    REG_INTCON  = 0x04,
    REG_IOCON   = 0x05,
    REG_GPPU    = 0x06,
    REG_INTF    = 0x07,
    REG_INTCAP  = 0x08,
    REG_GPIO    = 0x09,
    REG_OLAT    = 0x0A,

    SWITCH_BIT = 0,
    OUTPUT_BIT = 7,

    LED_HEARTBEAT_PIN = 25,
    BLINK_INTERVAL_MS = 250
};

static void i2c_write_register(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    uint8_t payload[2] = {regAddr, data};
    i2c_write_blocking(i2c_default, devAddr, payload, 2, false);
}

static uint8_t i2c_read_register(uint8_t devAddr, uint8_t regAddr) {
    uint8_t result;
    i2c_write_blocking(i2c_default, devAddr, &regAddr, 1, true);
    i2c_read_blocking(i2c_default, devAddr, &result, 1, false);
    return result;
}

static void expander_setup(uint8_t devAddr) {
    i2c_write_register(devAddr, REG_OLAT, 0x00);
    i2c_write_register(devAddr, REG_GPPU, 1 << SWITCH_BIT);
    i2c_write_register(devAddr, REG_IODIR, 0x7F);
}

static void hardware_init(void) {
    stdio_init_all();

    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    gpio_init(LED_HEARTBEAT_PIN);
    gpio_set_dir(LED_HEARTBEAT_PIN, GPIO_OUT);

    expander_setup(EXPANDER_ADDR);
}

static void heartbeat_tick(uint32_t currentTime) {
    static uint32_t prevToggleTime = 0;
    static bool ledState = false;

    if (currentTime - prevToggleTime >= BLINK_INTERVAL_MS) {
        prevToggleTime = currentTime;
        ledState = !ledState;
        gpio_put(LED_HEARTBEAT_PIN, ledState);
    }
}

static void button_led_tick(void) {
    uint8_t gpioState = i2c_read_register(EXPANDER_ADDR, REG_GPIO);
    bool switchActive = !((gpioState >> SWITCH_BIT) & 0x01);
    uint8_t outputValue = switchActive ? (1 << OUTPUT_BIT) : 0x00;

    i2c_write_register(EXPANDER_ADDR, REG_OLAT, outputValue);
}

int main(void) {
    hardware_init();

    for (;;) {
        heartbeat_tick(to_ms_since_boot(get_absolute_time()));
        button_led_tick();
        sleep_ms(10);
    }

    return 0;
}
