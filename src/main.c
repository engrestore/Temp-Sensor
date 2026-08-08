/* main.c
* TM4C123G LaunchPad + AHT10 (I2C) temperature/humidity logger.
*
* Wiring (AHT10 breakout -> LaunchPad):
*   VCC -> 3.3V
*   GND -> GND
*   SCL -> PB2
*   SDA -> PB3
*
* Output: temperature and humidity printed once per cycle over UART0
* (115200 8N1) via the LaunchPad's USB virtual COM port. On-board RGB
* LED turns red if temperature exceeds TEMP_THRESHOLD_X10, green otherwise.
*
* Clock: runs on the default 16 MHz internal oscillator (no PLL).
* CLOCK_SETUP is disabled in system_TM4C123.c so the chip actually stays
* at this default instead of the device pack's default 50 MHz PLL config
* every delay/baud-rate/I2C-timing calculation in this project assumes
* 16 MHz. The actual clock is printed at startup below as a sanity check.
*/
#include <stdint.h>
#include "tm4c123gh6pm_regs.h"
#include "SysTick.h"
#include "I2C0.h"
#include "AHT10.h"
#include "UART0.h"
#include "LED.h"

#define TEMP_THRESHOLD_X10  300   // 30.0 C

extern uint32_t SystemCoreClock;   // set by system_TM4C123.c / SystemCoreClockUpdate()

// Prints exactly what went wrong on an AHT10_ERR_I2C
static void PrintAHT10Error(uint8_t err)
{
    if (err == AHT10_ERR_BUSY) {
        UART0_OutString("busy/uncalibrated (timed out waiting)");
    } else if (err == AHT10_ERR_I2C) {
        switch (AHT10_LastI2CError()) {
            case I2C0_ERR_ADDR:    UART0_OutString("I2C address NACK (device not responding)"); break;
            case I2C0_ERR_DATA:    UART0_OutString("I2C data NACK");                             break;
            case I2C0_ERR_ARBLST:  UART0_OutString("I2C arbitration lost");                       break;
            case I2C0_ERR_TIMEOUT: UART0_OutString("I2C bus stuck (timeout -- check wiring/pull-ups)"); break;
            default:                UART0_OutString("I2C error");                                 break;
        }
    } else {
        UART0_OutString("unknown error");
    }
}

int main(void)
{
    int32_t temperature_x10;
    int32_t humidity_x10;
    uint8_t err;

    SysTick_Init();
    LED_Init();
    UART0_Init();
    I2C0_Init();

    UART0_OutString("\r\n--- TM4C123 + AHT10 Temp/Humidity Logger ---\r\n");

    UART0_OutString("Clock: ");
    UART0_OutFixed1((int32_t)(SystemCoreClock / 100000));
    UART0_OutString(" MHz\r\n");

    err = AHT10_Init();
    if (err != AHT10_OK) {
        UART0_OutString("AHT10 init failed: ");
        PrintAHT10Error(err);
        UART0_OutString(". Check wiring (PB2=SCL, PB3=SDA, pull-ups).\r\n");
    }

    while (1) {
        err = AHT10_Read(&temperature_x10, &humidity_x10);

        if (err == AHT10_OK) {
            UART0_OutString("Temp: ");
            UART0_OutFixed1(temperature_x10);
            UART0_OutString(" C   Humidity: ");
            UART0_OutFixed1(humidity_x10);
            UART0_OutString(" %\r\n");

            if (temperature_x10 > TEMP_THRESHOLD_X10) {
                LED_Red();
            } else {
                LED_Green();
            }
        } else {
            UART0_OutString("Sensor read error: ");
            PrintAHT10Error(err);
            UART0_OutString("\r\n");
            LED_Blue();
        }

        SysTick_Wait1ms(1800);
    }
}