# TM4C123 + AHT10 Temperature/Humidity Logger

Bare-metal (register-level) firmware for the TI TM4C123G LaunchPad that reads
temperature and humidity from an AHT10 I2C sensor and logs it over a serial
connection, with the on-board RGB LED giving an at-a-glance status.

## Hardware

- TI TM4C123G LaunchPad (EK-TM4C123GXL)
- AHT10 temperature/humidity breakout board

### Wiring

| AHT10 | LaunchPad |
|-------|-----------|
| VCC   | 3.3V      |
| GND   | GND       |
| SCL   | PB2       |
| SDA   | PB3       |

## Behavior

- Prints a startup banner and the detected system clock speed over UART0
- Every ~2 seconds: triggers a sensor reading and prints temperature (C)
  and relative humidity (%).
- On-board RGB LED:
  - Green -- last reading OK, temperature at or below the threshold
    (30.0C by default)
  - Red -- last reading OK, temperature above the threshold
  - Blue -- last reading failed (I2C error or sensor busy/timeout;
    the specific cause is printed over UART)

## Building

1. Open the project in Keil MDK-ARM (uVision5).
2. Make sure the **Tiva C Series Device Family Pack** is installed via
   Pack Installer (search "TM4C").
3. In `system_TM4C123.c`, `CLOCK_SETUP` must be `0`. This keeps the chip
   at its default 16 MHz internal oscillator, which every timing
   calculation in this project (UART baud rate, I2C clock, SysTick delays)
   assumes.
4. Build (F7). This produces `New.axf`, and a post-build step
   (`fromelf --bin -o New.bin New.axf`) produces `New.bin` for flashing.

## Flashing

Keil's built-in "Stellaris ICDI" debug driver is unreliable on modern
Windows and unsupported on Windows on ARM. This project flashes via
OpenOCD instead, using the ICDI's JTAG interface.

Run:

```powershell
.\flash.ps1
```

After flashing, **press the LaunchPad's physical RESET button** -- the
ICDI's automatic reset-after-programming step doesn't work reliably, so
the chip is left halted after flashing rather than auto-restarting.

## Monitoring output

Connect a serial terminal (PuTTY, Tera Term, etc.) to the LaunchPad's
virtual COM port at 115200 8N1, no flow control. Find the COM port number
in Windows Device Manager under "Ports (COM & LPT)".
