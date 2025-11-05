#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "pulse.pio.h"
#include "pico_timer.h"
#define min_v 9

void pulse_setup(){
    // PIO setup
    const uint PIN_BASE = 28;
    const float CLKDIV = 625.f;  // ≈ 40kHz
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pulse_program);
    pulse_program_init(pio, sm, offset, PIN_BASE, CLKDIV);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_put_blocking(pio, sm, 8);  // Send pulse pairs
    pio_sm_set_enabled(pio, sm, true);
    //printf("Pulses triggered...\n");
    uint32_t done = pio_sm_get_blocking(pio, sm);
    //printf("Pulse sequence completed. PIO response: %u\n", done);
}


uint16_t read_stable_adc(int samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += adc_read();
    }
    return sum / samples;
}
int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26); 
    adc_select_input(0);

    while (1) {
        uint64_t t1 = 0;
        uint8_t raw = 10;
        const int max_samples = 20000;
        pulse_setup();
        uint64_t t0 = read_timer_raw_macro();
        for (int i = 0; i < max_samples; i++){
            raw = read_stable_adc(2);
            //sleep_us(1);
            //printf("raw value = %u\n", raw);
            if(raw < min_v){
                t1 = read_timer_raw_macro();
                //printf("Threshold crossed: raw=%u at dt=%llu us\n", raw, (t1 - t0));
                float dt = (t1 - t0 - 200);
                float distance = (dt * 0.0344)/2;
                printf("distance = %.2f cm\n", distance);
                break;
            }
        }
        if (t1 == 0){
            //printf("No threshold crossing detected\n");
        }
        sleep_ms(10);
    }
}



