#include "pico/stdlib.h"

#define TL 0x40054024
#define TH 0x40054028

//--------------------------------------------------------------
// unsafe timer for readings under 1 second, this can be useful for very fast sampling
// can give corrupted data, only to be used on safe nonfailure critical hardware
uint64_t read_timer_LT() {
    volatile uint32_t* TIMERAWL = (uint32_t*)TL;
    volatile uint32_t* TIMERAWH = (uint32_t*)TH;
    return ((uint64_t)(*TIMERAWH) << 32) | (*TIMERAWL);
}
//---------------------------------------------------------------
// not using macros making it rather fast
//using RP2040 2 32 bit adresses to acess the 64 bit value
// ADRESSES FOR 64 BIT TIMER

//   |--------------------|
//   |TIMERL = 0x40054024 |
//   |TIMERH = 0x40054028 |
//   |--------------------|

// gives safe readings doesent allow the timer to overflow
uint64_t read_timer_raw() {
    volatile uint32_t* TIMERAWL = (uint32_t*)TL;
    volatile uint32_t* TIMERAWH = (uint32_t*)TH;
    uint32_t hi1, lo, hi2;

    do {
        hi1 = *TIMERAWH;
        lo  = *TIMERAWL;
        hi2 = *TIMERAWH;
    } while (hi1 != hi2);

    return ((uint64_t)hi1 << 32) | lo;
}
