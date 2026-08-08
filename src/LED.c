// LED.c
#include "tm4c123gh6pm_regs.h"
#include "LED.h"

#define LED_RED   0x02  // PF1
#define LED_BLUE  0x04  // PF2
#define LED_GREEN 0x08  // PF3
#define LED_ALL   (LED_RED | LED_BLUE | LED_GREEN)

void LED_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;  // enable clock for GPIO Port F (bit5)
    { volatile int d; for (d = 0; d < 10; d++) {} }

    // PF1, PF2, PF3 are not locked pins (unlike PF0), so no unlock needed.
    GPIO_PORTF_DIR_R |= LED_ALL;   // outputs
    GPIO_PORTF_AFSEL_R &= ~LED_ALL; // plain GPIO, not alternate function
    GPIO_PORTF_DEN_R |= LED_ALL;   // digital enable

    LED_Off();
}

void LED_Off(void)
{
    GPIO_PORTF_DATA_R &= ~LED_ALL;
}

void LED_Red(void)
{
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_ALL) | LED_RED;
}

void LED_Green(void)
{
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_ALL) | LED_GREEN;
}

void LED_Blue(void)
{
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_ALL) | LED_BLUE;
}