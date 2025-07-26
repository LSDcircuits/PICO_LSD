#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define TL (*(volatile uint32_t*)(0x40054024))
#define TH (*(volatile uint32_t*)(0x40054028))


uint64_t read_timer_raw_macro() {
    uint32_t hi1, lo, hi2;
    do {
        hi1 = timer_hw->timerawh;
        lo = timer_hw->timerawl;
        hi2 = timer_hw->timerawh;
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}

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
    uint32_t hi1, lo, hi2;
    do {
        hi1 = TH;
        lo  = TL;
        hi2 = TH;
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}
