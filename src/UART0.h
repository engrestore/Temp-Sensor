/*
* UART0.h
* Register-level UART0 driver, 115200 baud, 8N1, over the LaunchPad's
* USB virtual COM port (pins PA0=U0RX, PA1=U0TX are routed through the
* on-board ICDI debugger's USB-UART bridge -- no extra wiring needed).
* Assumes 16 MHz system clock (no PLL).
*/

#ifndef UART0_H
#define UART0_H

#include <stdint.h>

void UART0_Init(void);

// Sends one character. If the transmit FIFO never drains (bad/absent
// USB-UART bridge, etc.) this gives up after a bounded wait instead of
// hanging forever -- the character is dropped rather than blocking the
// whole program.
void UART0_OutChar(char c);

void UART0_OutString(const char *s);

// Print a signed fixed-point value with one decimal digit, e.g. valueX10=-235 -> "-23.5"
void UART0_OutFixed1(int32_t valueX10);

#endif