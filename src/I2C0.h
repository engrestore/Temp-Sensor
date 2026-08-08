/*
* I2C0.h
* Bare-metal (register-level) I2C0 master driver.
* Pins: PB2 = I2C0SCL, PB3 = I2C0SDA (AF3)
* Assumes 16 MHz system clock -> configured for 100 kHz standard-mode I2C.
*/

#ifndef I2C0_H
#define I2C0_H

#include <stdint.h>

// Return codes
#define I2C0_OK          0
#define I2C0_ERR_ADDR    1   // no ACK on address byte (device not present/wrong addr)
#define I2C0_ERR_DATA    2   // no ACK on a data byte
#define I2C0_ERR_ARBLST  3   // arbitration lost
#define I2C0_ERR_TIMEOUT 4   // bus stayed BUSY too long (stuck bus, bad wiring, dead device)

void I2C0_Init(void);

// Write 'length' bytes from 'data' to the slave at 'slaveAddr' (7-bit address).
uint8_t I2C0_Write(uint8_t slaveAddr, const uint8_t *data, uint8_t length);

// Read 'length' bytes from the slave at 'slaveAddr' into 'data'.
uint8_t I2C0_Read(uint8_t slaveAddr, uint8_t *data, uint8_t length);

#endif