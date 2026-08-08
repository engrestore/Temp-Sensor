/*
* SysTick.h
* Simple polled millisecond delay using the Cortex-M4 SysTick timer.
* Assumes the default 16 MHz PIOSC system clock (no PLL configured).
*/

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void SysTick_Init(void);
void SysTick_Wait(uint32_t delay);      // wait 'delay' clock cycles (max 2^24)
void SysTick_Wait1ms(uint32_t delayMs); // wait 'delayMs' milliseconds

#endif