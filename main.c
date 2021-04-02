#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "ws2812.pio.h"

#define PIN_TX 22
#define N_PIXELS 16

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

int main() {
    stdio_init_all();
    adc_init();

    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, PIN_TX, 800000, false); //rgb

    uint32_t pixels[N_PIXELS] = {0};
    sleep_ms(2000);

    while (1) {
        long sound_intensity = 0;
        for(int i=0; i<32; i++) {
            sound_intensity += adc_read();
        }
        sound_intensity >>= 5;

        pixels[3]  = pixels[2];
        pixels[4]  = pixels[5];
        pixels[11] = pixels[10];
        pixels[15] = pixels[14];

        pixels[2]  = pixels[1];
        pixels[5]  = pixels[6];
        pixels[10] = pixels[9];
        pixels[14] = pixels[13];

        pixels[1]  = pixels[0];
        pixels[6]  = pixels[7];
        pixels[9]  = pixels[8];
        pixels[13] = pixels[12];

        if (sound_intensity >= 2200) {
            pixels[0]  = rand();
            pixels[7]  = rand();
            pixels[8]  = rand();
            pixels[12] = rand();
        } else if (sound_intensity >= 1700) {
            pixels[0]  = rand();
            pixels[7]  = rand();
            pixels[8]  = rand();
            pixels[12] = urgb_u32(0, 0, 0);
        } else if (sound_intensity >= 1250) {
            pixels[0]  = rand();
            pixels[7]  = rand();
            pixels[8]  = urgb_u32(0, 0, 0);
            pixels[12] = urgb_u32(0, 0, 0);
        } else if (sound_intensity >= 800) {
            pixels[0]  = rand();
            pixels[7]  = urgb_u32(0, 0, 0);
            pixels[8]  = urgb_u32(0, 0, 0);
            pixels[12] = urgb_u32(0, 0, 0);
        } else {
            pixels[0]  = urgb_u32(0, 0, 0);
            pixels[7]  = urgb_u32(0, 0, 0);
            pixels[8]  = urgb_u32(0, 0, 0);
            pixels[12] = urgb_u32(0, 0, 0);
        }

        for (uint8_t i = 0; i < N_PIXELS; i++) {
            put_pixel(pixels[i]);
        }

        sleep_ms(10);
    }
}
