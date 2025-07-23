#ifndef PICO_TIMER_H
#define PICO_TIMER_H
#include <stdio.h>
#include "pico/stdlib.h"

#define TL 0x40054024
#define TH 0x40054028

uint64_t read_timer_LT();
uint64_t read_timer_raw();

#endif // PICO_TIMER_H




