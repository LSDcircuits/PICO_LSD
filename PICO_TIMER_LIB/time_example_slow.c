#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    adc_init();
    adc_select_input(0);  // use ADC0

    // First measurement
    absolute_time_t t1 = get_absolute_time();
    uint16_t val1 = adc_read();

    // Simulate delay or do something else
    sleep_ms(100);  // pretend you're waiting for next measurement

    // Second measurement
    absolute_time_t t2 = get_absolute_time();
    uint16_t val2 = adc_read();

    // Calculate time difference in microseconds
    int64_t delta_us = absolute_time_diff_us(t1, t2);

    printf("Time between measurements: %lld microseconds\n", delta_us);

    return 0;
}
