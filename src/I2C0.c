// I2C0.c
#include "tm4c123gh6pm_regs.h"
#include "I2C0.h"

/*
* Max spin count while waiting for BUSY to clear. This is a generous
* upper bound for a healthy 100 kHz bus (a normal transaction phase
* completes in well under this many loop iterations); if we ever hit
* this limit the bus is stuck (bad wiring, missing pull-ups, dead/absent
* device holding SDA or SCL low, etc.) and we bail out instead of
* hanging the whole program forever.
*/

#define I2C0_BUSY_TIMEOUT   50000

// Wait for the current I2C0 master transaction to finish (polled, no interrupts).
// Returns I2C0_OK if BUSY cleared normally, or I2C0_ERR_TIMEOUT if the bus
// never went idle within I2C0_BUSY_TIMEOUT iterations.
static uint8_t I2C0_WaitBusy(void)
{
    uint32_t timeout = I2C0_BUSY_TIMEOUT;

    while (I2C0_MCS_R & I2C_MCS_BUSY) {
        if (--timeout == 0) {
            return I2C0_ERR_TIMEOUT;
        }
    }
    return I2C0_OK;
}

// Translate MCS status bits into one of our error codes.
// Returns I2C0_OK if no error occurred.
static uint8_t I2C0_CheckError(void)
{
    uint32_t status = I2C0_MCS_R;
    if (status & I2C_MCS_ARBLST) return I2C0_ERR_ARBLST;
    if (status & I2C_MCS_ADRACK) return I2C0_ERR_ADDR;
    if (status & I2C_MCS_DATACK) return I2C0_ERR_DATA;
    return I2C0_OK;
}

void I2C0_Init(void)
{
    // 1. Enable clocks: I2C0 module and GPIO port B
    SYSCTL_RCGCI2C_R  |= 0x01;   // I2C0
    SYSCTL_RCGCGPIO_R |= 0x02;   // Port B (bit1)

    // Wait until both peripherals actually report ready before touching
    // their registers.
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) { /* wait for GPIO Port B ready */ }
    while ((SYSCTL_PRI2C_R  & 0x01) == 0) { /* wait for I2C0 ready */ }

    // 2. Configure PB2 (SCL) / PB3 (SDA) for I2C0 alternate function (AF3)
    GPIO_PORTB_AFSEL_R |= 0x0C;                              // enable AF on PB2, PB3
    GPIO_PORTB_ODR_R   |= 0x08;                               // open-drain on SDA (PB3) only
    GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300; // AF3 for PB2,PB3
    GPIO_PORTB_DEN_R   |= 0x0C;                               // digital enable PB2, PB3

    // 3. Enable I2C0 as master
    I2C0_MCR_R = I2C_MCR_MFE;

    // 4. Set the SCL clock rate to 100 kHz.
    //    TPR = (SysClk / (2*(SCL_LP+SCL_HP)*SCL_freq)) - 1
    //        = (16,000,000 / (2*10*100,000)) - 1 = 7   (16 MHz system clock)
    I2C0_MTPR_R = 7;
}

uint8_t I2C0_Write(uint8_t slaveAddr, const uint8_t *data, uint8_t length)
{
    uint8_t err;
    uint8_t i;
    if (length == 0) return I2C0_ERR_DATA;

    I2C0_MSA_R = (slaveAddr << 1) | 0;   // R/S = 0 (write)
    I2C0_MDR_R = data[0];

    if (length == 1) {
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
        err = I2C0_WaitBusy();
        if (err != I2C0_OK) return err;
        return I2C0_CheckError();
    }

    // first byte
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;
    err = I2C0_WaitBusy();
    if (err != I2C0_OK) return err;
    err = I2C0_CheckError();
    if (err != I2C0_OK) return err;

    // middle bytes
    for (i = 1; i < length - 1; i++) {
        I2C0_MDR_R = data[i];
        I2C0_MCS_R = I2C_MCS_RUN;
        err = I2C0_WaitBusy();
        if (err != I2C0_OK) return err;
        err = I2C0_CheckError();
        if (err != I2C0_OK) return err;
    }

    // last byte
    I2C0_MDR_R = data[length - 1];
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
    err = I2C0_WaitBusy();
    if (err != I2C0_OK) return err;
    return I2C0_CheckError();
}

uint8_t I2C0_Read(uint8_t slaveAddr, uint8_t *data, uint8_t length)
{
    uint8_t err;
    uint8_t i;
    if (length == 0) return I2C0_ERR_DATA;

    I2C0_MSA_R = (slaveAddr << 1) | 1;   // R/S = 1 (read)

    if (length == 1) {
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
        err = I2C0_WaitBusy();
        if (err != I2C0_OK) return err;
        err = I2C0_CheckError();
        if (err != I2C0_OK) return err;
        data[0] = (uint8_t)I2C0_MDR_R;
        return I2C0_OK;
    }

    // first byte: START + RUN + ACK (tell slave more bytes follow)
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK;
    err = I2C0_WaitBusy();
    if (err != I2C0_OK) return err;
    err = I2C0_CheckError();
    if (err != I2C0_OK) return err;
    data[0] = (uint8_t)I2C0_MDR_R;

    // middle bytes: RUN + ACK
    for (i = 1; i < length - 1; i++) {
        I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_ACK;
        err = I2C0_WaitBusy();
        if (err != I2C0_OK) return err;
        err = I2C0_CheckError();
        if (err != I2C0_OK) return err;
        data[i] = (uint8_t)I2C0_MDR_R;
    }

    // last byte: RUN + STOP, no ACK (NACK tells slave this is the last byte)
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
    err = I2C0_WaitBusy();
    if (err != I2C0_OK) return err;
    err = I2C0_CheckError();
    if (err != I2C0_OK) return err;
    data[length - 1] = (uint8_t)I2C0_MDR_R;

    return I2C0_OK;
}