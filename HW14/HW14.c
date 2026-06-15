#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#define HX711_SCK_PIN 14
#define HX711_DT_PIN  15

#define IIR_A 0.8f

#define MAX_SAMPLES 4000

int32_t force_raw[MAX_SAMPLES];
float force_filt[MAX_SAMPLES];
uint32_t sample_ms[MAX_SAMPLES];

void hx711_init(void)
{
    gpio_init(HX711_SCK_PIN);
    gpio_set_dir(HX711_SCK_PIN, GPIO_OUT);
    gpio_put(HX711_SCK_PIN, 0);

    gpio_init(HX711_DT_PIN);
    gpio_set_dir(HX711_DT_PIN, GPIO_IN);
}

int32_t hx711_read(void)
{
    while (gpio_get(HX711_DT_PIN)) {
        tight_loop_contents();
    }

    uint32_t bits = 0;
    for (int i = 0; i < 24; i++) {
        gpio_put(HX711_SCK_PIN, 1);
        sleep_us(1);
        bits = (bits << 1) | gpio_get(HX711_DT_PIN);
        gpio_put(HX711_SCK_PIN, 0);
        sleep_us(1);
    }

    gpio_put(HX711_SCK_PIN, 1);
    sleep_us(1);
    gpio_put(HX711_SCK_PIN, 0);
    sleep_us(1);

    if (bits & 0x800000) {
        bits |= 0xFF000000;
    }
    return (int32_t)bits;
}

int main()
{
    stdio_init_all();
    hx711_init();

    while (true) {
        int count;
        if (scanf("%d", &count) != 1) {
            continue;
        }
        if (count < 1) count = 1;
        if (count > MAX_SAMPLES) count = MAX_SAMPLES;

        float avg = 0.0f;
        for (int idx = 0; idx < count; idx++) {
            int32_t sample = hx711_read();
            if (idx == 0) {
                avg = (float)sample;
            } else {
                avg = IIR_A * avg + (1.0f - IIR_A) * (float)sample;
            }
            force_raw[idx] = sample;
            force_filt[idx] = avg;
            sample_ms[idx] = to_ms_since_boot(get_absolute_time());
        }

        for (int idx = 0; idx < count; idx++) {
            printf("%ld,%.2f,%lu\n", (long)force_raw[idx], force_filt[idx], (unsigned long)sample_ms[idx]);
        }
    }
}
