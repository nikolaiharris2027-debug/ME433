#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

void set_servo_angle(int angle_deg);
void sweep_servo(int start_deg, int end_deg, int step_deg);

#define SERVO_PWM_GPIO 16

bool adc_sample_callback(__unused struct repeating_timer *t) {
    uint16_t raw_adc = adc_read();
    printf("%f\r\n", (float)raw_adc / 4095 * 3.3);
    return true;
}

int main()
{
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // periodic ADC sampling
    struct repeating_timer sample_timer;
    add_repeating_timer_ms(-100, adc_sample_callback, NULL, &sample_timer);

    // servo PWM setup
    gpio_set_function(SERVO_PWM_GPIO, GPIO_FUNC_PWM);
    uint pwm_slice = pwm_gpio_to_slice_num(SERVO_PWM_GPIO);
    float clk_div = 50;
    pwm_set_clkdiv(pwm_slice, clk_div);
    uint16_t pwm_wrap = 60000;
    pwm_set_wrap(pwm_slice, pwm_wrap);
    pwm_set_enabled(pwm_slice, true);
    pwm_set_gpio_level(SERVO_PWM_GPIO, 0);

    // ADC input setup
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // main sweep loop
    while (true) {
        sweep_servo(10, 170, 1);
        sweep_servo(170, 10, -1);
    }
}

void sweep_servo(int start_deg, int end_deg, int step_deg) {
    for (int pos = start_deg; pos != end_deg; pos += step_deg) {
        set_servo_angle(pos);
        sleep_ms(10);
    }
}

void set_servo_angle(int angle_deg) {
    pwm_set_gpio_level(SERVO_PWM_GPIO, (int)((0.03 + (angle_deg / 180.0) * 0.1) * 60000));
}
