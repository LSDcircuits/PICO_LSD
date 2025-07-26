#include <stdio.h>
#include "pico/stdlib.h"
#include "pico_timer.h"

int main() {
    stdio_init_all();
    sleep_ms(2000); // Give time for USB to settle

    while (1) {
        uint64_t t0 = read_timer_raw_macro();
        sleep_us(10);
        uint64_t t1 = read_timer_raw_macro();

        uint64_t dt = t1 - t0;
        printf("Elapsed time: %llu microseconds\n", dt);  // Should be ~100–500 µs

        sleep_ms(1000);
    }
}


    
