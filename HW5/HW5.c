#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

#define PIN_I2C_SDA 20
#define PIN_I2C_SCL 21

#define MPU_REG_CONFIG       0x1A
#define MPU_REG_GYRO_CONFIG  0x1B
#define MPU_REG_ACCEL_CONFIG 0x1C
#define MPU_REG_PWR_MGMT_1   0x6B
#define MPU_REG_PWR_MGMT_2   0x6C
#define MPU_REG_ACCEL_XOUT_H 0x3B
#define MPU_REG_ACCEL_XOUT_L 0x3C
#define MPU_REG_ACCEL_YOUT_H 0x3D
#define MPU_REG_ACCEL_YOUT_L 0x3E
#define MPU_REG_ACCEL_ZOUT_H 0x3F
#define MPU_REG_ACCEL_ZOUT_L 0x40
#define MPU_REG_TEMP_OUT_H   0x41
#define MPU_REG_TEMP_OUT_L   0x42
#define MPU_REG_GYRO_XOUT_H  0x43
#define MPU_REG_GYRO_XOUT_L  0x44
#define MPU_REG_GYRO_YOUT_H  0x45
#define MPU_REG_GYRO_YOUT_L  0x46
#define MPU_REG_GYRO_ZOUT_H  0x47
#define MPU_REG_GYRO_ZOUT_L  0x48
#define MPU_REG_WHO_AM_I     0x75

typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t temp_raw;
    int16_t gyro_x, gyro_y, gyro_z;
} mpu_sample_t;

static uint8_t mpu_address = 0x68;

static void mpu_write_byte(uint8_t reg, uint8_t value) {
    uint8_t packet[2] = {reg, value};
    i2c_write_blocking(i2c_default, mpu_address, packet, 2, false);
}

static uint8_t mpu_read_byte(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(i2c_default, mpu_address, &reg, 1, true);
    i2c_read_blocking(i2c_default, mpu_address, &value, 1, false);
    return value;
}

static void mpu_read_sample(mpu_sample_t *sample) {
    uint8_t reg = MPU_REG_ACCEL_XOUT_H;
    uint8_t raw[14];
    i2c_write_blocking(i2c_default, mpu_address, &reg, 1, true);
    i2c_read_blocking(i2c_default, mpu_address, raw, 14, false);
    sample->accel_x  = (int16_t)((raw[0]  << 8) | raw[1]);
    sample->accel_y  = (int16_t)((raw[2]  << 8) | raw[3]);
    sample->accel_z  = (int16_t)((raw[4]  << 8) | raw[5]);
    sample->temp_raw = (int16_t)((raw[6]  << 8) | raw[7]);
    sample->gyro_x   = (int16_t)((raw[8]  << 8) | raw[9]);
    sample->gyro_y   = (int16_t)((raw[10] << 8) | raw[11]);
    sample->gyro_z   = (int16_t)((raw[12] << 8) | raw[13]);
}

static void mpu_configure(void) {
    mpu_address = 0x68;
    uint8_t id = mpu_read_byte(MPU_REG_WHO_AM_I);
    if (id != 0x68 && id != 0x98) {
        mpu_address = 0x69;
        id = mpu_read_byte(MPU_REG_WHO_AM_I);
    }
    printf("IMU WHO_AM_I=0x%02X at addr=0x%02X\n", id, mpu_address);
    if (id != 0x68 && id != 0x98) {
        printf("IMU not found, continuing anyway\n");
        return;
    }
    mpu_write_byte(MPU_REG_PWR_MGMT_1, 0x00);
    mpu_write_byte(MPU_REG_ACCEL_CONFIG, 0x00);
    mpu_write_byte(MPU_REG_GYRO_CONFIG, 0x18);
}

static void draw_char(int x, int y, char c) {
    if (c < 0x20 || c > 0x7f) return;
    int glyph = c - 0x20;
    for (int col = 0; col < 5; col++) {
        unsigned char bits = FONT_TABLE[glyph][col];
        for (int row = 0; row < 8; row++) {
            oled_set_pixel(x + col, y + row, (bits >> row) & 1);
        }
    }
}

static void draw_text(int x, int y, const char *text) {
    while (*text != '\0') {
        draw_char(x, y, *text);
        x += 6;
        text++;
    }
}

static void draw_line(int x0, int y0, int x1, int y1) {
    int delta_x = abs(x1 - x0), step_x = x0 < x1 ? 1 : -1;
    int delta_y = -abs(y1 - y0), step_y = y0 < y1 ? 1 : -1;
    int error = delta_x + delta_y;
    while (1) {
        oled_set_pixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;
        int error2 = 2 * error;
        if (error2 >= delta_y) { error += delta_y; x0 += step_x; }
        if (error2 <= delta_x) { error += delta_x; y0 += step_y; }
    }
}

static inline int clamp_int(int value, int low, int high) {
    return value < low ? low : value > high ? high : value;
}

static void setup_i2c(void) {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}

static void scan_i2c_bus(void) {
    printf("I2C scan:\n");
    for (int address = 1; address < 128; address++) {
        uint8_t dummy;
        int result = i2c_read_blocking(i2c_default, address, &dummy, 1, false);
        if (result >= 0) printf("  device at 0x%02X\n", address);
    }
    printf("scan done\n");
}

static void draw_gravity_vector(const mpu_sample_t *sample) {
    const int origin_x = 64, origin_y = 16;
    int tip_x = clamp_int(origin_x - sample->accel_x * 60 / 16384, 0, 127);
    int tip_y = clamp_int(origin_y + sample->accel_y * 14 / 16384, 0, 31);

    oled_clear();
    draw_line(origin_x, origin_y, tip_x, tip_y);
    oled_set_pixel(origin_x,     origin_y,     1);
    oled_set_pixel(origin_x + 1, origin_y,     1);
    oled_set_pixel(origin_x,     origin_y + 1, 1);
    oled_set_pixel(origin_x + 1, origin_y + 1, 1);
    oled_flush();
}

int main() {
    stdio_init_all();
    printf("USB connected\n");

    setup_i2c();
    printf("I2C ok\n");

    oled_init();
    printf("OLED ok\n");

    scan_i2c_bus();

    mpu_configure();
    printf("IMU ok\n");

    absolute_time_t next_frame = get_absolute_time();

    while (true) {
        mpu_sample_t sample;
        mpu_read_sample(&sample);

        draw_gravity_vector(&sample);

        sleep_until(next_frame);
    }
}
