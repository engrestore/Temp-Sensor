// SysTick.c
#include "tm4c123gh6pm_regs.h"
#include "SysTick.h"
 
void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;                 // disable SysTick during setup
    NVIC_ST_RELOAD_R = 0x00FFFFFF;      // max reload value
    NVIC_ST_CURRENT_R = 0;              // clear current value
    // enable SysTick, use system clock, no interrupt (polled)
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC;
}
 
// Wait for 'delay' bus clock cycles (delay must be < 2^24)
void SysTick_Wait(uint32_t delay)
{
    NVIC_ST_RELOAD_R = delay - 1;
    NVIC_ST_CURRENT_R = 0;              // any write clears COUNT flag
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0) {
        // wait for COUNT flag (set when timer reaches 0)
    }
}
 
// Wait for 'delayMs' milliseconds, assuming 16 MHz system clock
// (16,000,000 cycles/sec / 1000 = 16,000 cycles/ms)
void SysTick_Wait1ms(uint32_t delayMs)
{
    uint32_t i;
    for (i = 0; i < delayMs; i++) {
        SysTick_Wait(16000);
    }
}
 