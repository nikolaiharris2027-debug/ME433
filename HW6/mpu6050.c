#include "mpu6050.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define MPU6050_ADDR        0x68
#define MPU6050_I2C_PORT    i2c0
#define MPU6050_I2C_BAUDRATE (400 * 1000)

#define REG_PWR_MGMT_1      0x6B
#define REG_ACCEL_XOUT_H    0x3B

static void mpu6050_write_reg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, buf, 2, false);
}

void mpu6050_init(void) {
    i2c_init(MPU6050_I2C_PORT, MPU6050_I2C_BAUDRATE);

    gpio_set_function(MPU6050_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(MPU6050_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(MPU6050_SDA_PIN);
    gpio_pull_up(MPU6050_SCL_PIN);

    // Wake the device up (default is sleep mode)
    mpu6050_write_reg(REG_PWR_MGMT_1, 0x00);
}

void mpu6050_read(mpu6050_data_t *d) {
    uint8_t reg = REG_ACCEL_XOUT_H;
    uint8_t buf[14];

    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, buf, 14, false);

    d->ax = (int16_t)((buf[0]  << 8) | buf[1]);
    d->ay = (int16_t)((buf[2]  << 8) | buf[3]);
    d->az = (int16_t)((buf[4]  << 8) | buf[5]);
    d->temp = (int16_t)((buf[6] << 8) | buf[7]);
    d->gx = (int16_t)((buf[8]  << 8) | buf[9]);
    d->gy = (int16_t)((buf[10] << 8) | buf[11]);
    d->gz = (int16_t)((buf[12] << 8) | buf[13]);
}
