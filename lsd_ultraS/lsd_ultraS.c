#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "pulse.pio.h"
#include "pico_timer.h"
#define min_v 10


void pulse_setup(){
    // PIO setup
    const uint PIN_BASE = 16;
    const float CLKDIV = 652.f;  // ≈ 40kHz
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pulse_program);
    pulse_program_init(pio, sm, offset, PIN_BASE, CLKDIV);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_put_blocking(pio, sm, 8);  // Send 50 pulse pairs
    pio_sm_set_enabled(pio, sm, true);
    printf("Pulses triggered...\n");
    uint32_t done = pio_sm_get_blocking(pio, sm);
    printf("Pulse sequence completed. PIO response: %u\n", done);
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26); 
    adc_select_input(0);
    while (true) {
        pulse_setup();
        uint64_t t0 = read_timer_raw_macro();
        uint16_t raw = adc_read();
        if ( raw < min_v){
            uint64_t t1 = read_timer_raw_macro();
            uint64_t dt = t1 - t0;
            printf("raw=%u\n",raw);
        }break;
        pulse_setup();
        printf("Cycle complete. Waiting...\n\n");
        sleep_ms(10);  // 1 second delay between bursts
    }
}

// last update 8 pulse then read ADC, make software interrupt and use of cunter.
// make ranges for minimal circuit --- fun not practical
// make the ranges for the reliable - max distance. .. fak u if u reading this. ;) 
// PIO causes minor brain damage - Lorenzo 
