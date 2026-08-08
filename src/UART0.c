// UART0.c
#include "tm4c123gh6pm_regs.h"
#include "UART0.h"
/*
* Max spin count while waiting for the transmit FIFO to have room.
* Generous upper bound for a healthy link; if it's ever hit, something
* downstream (USB-UART bridge not enumerated, cable issue, etc.) isn't
* draining the FIFO, and we drop the byte instead of hanging forever.
*/
#define UART0_TX_TIMEOUT   50000

void UART0_Init(void)
{
    // 1. Enable clocks: UART0 module and GPIO port A
    SYSCTL_RCGCUART_R |= 0x01;   // UART0
    SYSCTL_RCGCGPIO_R |= 0x01;   // Port A (bit0)
    { volatile int d; for (d = 0; d < 10; d++) {} }

    // 2. Disable UART0 while configuring
    UART0_CTL_R &= ~UART_CTL_UARTEN;

    // 3. Baud rate = 115200, system clock = 16 MHz
    //    BRD = 16,000,000 / (16 * 115200) = 8.6805 -> IBRD=8, FBRD=round(0.6805*64)=44
    UART0_IBRD_R = 8;
    UART0_FBRD_R = 44;

    // 4. Frame: 8 data bits, no parity, 1 stop bit, FIFOs enabled
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    // 5. Configure PA0 (U0RX) / PA1 (U0TX) for UART alternate function (AF1)
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011; // AF1 for PA0,PA1
    GPIO_PORTA_DEN_R   |= 0x03;

    // 6. Enable UART0, transmit, receive
    UART0_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

void UART0_OutChar(char c)
{
    uint32_t timeout = UART0_TX_TIMEOUT;

    while (UART0_FR_R & UART_FR_TXFF) {
        // wait while transmit FIFO is full
        if (--timeout == 0) {
            return;   // give up, drop this character rather than hang forever
        }
    }
    UART0_DR_R = (uint32_t)c;
}

void UART0_OutString(const char *s)
{
    while (*s) {
        UART0_OutChar(*s);
        s++;
    }
}

// Prints a signed value that represents (real value * 10), e.g. -235 -> "-23.5"
void UART0_OutFixed1(int32_t valueX10)
{
    char buf[16];
    int i = 0;
    int32_t v = valueX10;
    uint32_t whole, frac;
    int neg = 0;

    if (v < 0) { neg = 1; v = -v; }
    whole = (uint32_t)(v / 10);
    frac  = (uint32_t)(v % 10);

    // build the digits of 'whole' in reverse, then reverse into buf
    {
        char tmp[12];
        int t = 0;
        if (whole == 0) {
            tmp[t++] = '0';
        } else {
            while (whole > 0) {
                tmp[t++] = (char)('0' + (whole % 10));
                whole /= 10;
            }
        }
        if (neg) buf[i++] = '-';
        while (t > 0) {
            buf[i++] = tmp[--t];
        }
    }
    buf[i++] = '.';
    buf[i++] = (char)('0' + frac);
    buf[i] = '\0';

    UART0_OutString(buf);
}