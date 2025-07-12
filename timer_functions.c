// made by Lorenzo Salvatore Daidone 
// here are just exmaples of timer functions to be used for integration 
// its a work in prograss to be a ble to in the future make faster code for my projects

// this one relies on the sdk macros 
uint64_t read_hw_timer_64() {
    uint32_t hi1, lo, hi2;
    do {
        hi1 = *(io_rw_32 *)(TIMER_BASE + TIMER_TIMERAWH_OFFSET);
        lo  = *(io_rw_32 *)(TIMER_BASE + TIMER_TIMERAWL_OFFSET);
        hi2 = *(io_rw_32 *)(TIMER_BASE + TIMER_TIMERAWH_OFFSET);
    } while (hi1 != hi2);
    return ((uint64_t)hi1 << 32) | lo;
}
//--------------------------------------------------------------


// not using macros making it rather fast
//using RP2040 2 32 bit adresses to acess the 64 bit value
// ADRESSES FOR 64 BIT TIMER

//   |--------------------|
//   |TIMERL = 0x40054024 |
//   |TIMERH = 0x40054028 |
//   |--------------------|

// gives safe readings doesent allow the timer to overflow
uint64_t read_timer_raw() {
    volatile uint32_t* TIMERAWL = (uint32_t*)0x40054024;
    volatile uint32_t* TIMERAWH = (uint32_t*)0x40054028;
    uint32_t hi1, lo, hi2;

    do {
        hi1 = *TIMERAWH;
        lo  = *TIMERAWL;
        hi2 = *TIMERAWH;
    } while (hi1 != hi2);

    return ((uint64_t)hi1 << 32) | lo;
}
//--------------------------------------------------------------
// unsafe timer for readings under 1 second, this can be useful for very fast sampling
// can give corrupted data, only to be used on safe nonfailure critical hardware
uint64_t read_timer_unsafe() {
    volatile uint32_t* TIMERAWL = (uint32_t*)0x40054024;
    volatile uint32_t* TIMERAWH = (uint32_t*)0x40054028;
    return ((uint64_t)(*TIMERAWH) << 32) | (*TIMERAWL);
}
//---------------------------------------------------------------
