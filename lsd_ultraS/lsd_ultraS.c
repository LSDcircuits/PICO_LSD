#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pulse.pio.h"  // side-set based dual-pulse generator



int main() {
    stdio_init_all();

    // ---- ADC Setup ----
    adc_init();
    adc_gpio_init(26);      // GPIO26 = ADC channel 0
    adc_select_input(0);    // Use ADC0 (GPIO26)

    // ---- PIO Pulse Setup ----
    const uint PIN_BASE = 16;    // Using GPIO16 and GPIO17
    const float pio_div = 781;
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pulse_program);

    while (true){
        pulse_program_init(pio, sm, offset, PIN_BASE, pio_div);
        pio_sm_set_enabled(pio, sm, false);  
        pio_sm_set_enabled(pio, sm, true);
        sleep_ms(100);
        uint16_t raw = adc_read();                    // 0–4095
        uint32_t mv = (raw * 3300) / 4095;            // Convert to mV
        printf("ADC raw: %u, voltage: %u mV\n", raw, mv);
        sleep_ms(1);  // Sampling delay
        pio_sm_set_enabled(pio, sm, false);  
        }        // ---- Main Loop ----

}
