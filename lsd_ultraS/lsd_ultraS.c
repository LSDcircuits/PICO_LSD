#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pulse.pio.h"
#include "pulse_inverted.pio.h"

int main() {
    stdio_init_all();

    const uint PIN1 = 2;
    const uint PIN2 = 3;
    const float pio_dev = 1000;

    PIO pio = pio0;
    uint sm0 = 0;
    uint sm1 = 1;

    uint offset1 = pio_add_program(pio, &pulse_program);
    uint offset2 = pio_add_program(pio, &pulse_inverted_program); // Can reuse same offset too

    float clk_div = pio_dev;

    // Init both SMs
    pulse_program_init(pio, sm0, offset1, PIN1, clk_div);
    pulse_inverted_program_init(pio, sm1, offset2, PIN2, clk_div);
    pio_sm_set_enabled(pio, sm0, false);
    pio_sm_set_enabled(pio, sm1, false);
    // Start both state machines (ideally simultaneously)

    
    pio_sm_set_enabled(pio, sm1, true);
    sleep_us(2);
    pio_sm_set_enabled(pio, sm0, true);

    while (true) {
        tight_loop_contents();
    }
}