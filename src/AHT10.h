/*
* AHT10.h
* Driver for the AHT10 I2C temperature/humidity sensor.
* I2C address: 0x38 (7-bit).
*/

#ifndef AHT10_H
#define AHT10_H

#include <stdint.h>

#define AHT10_I2C_ADDR   0x38

// Return codes
#define AHT10_OK         0
#define AHT10_ERR_I2C    1   // I2C bus error -- call AHT10_LastI2CError() for detail
#define AHT10_ERR_BUSY   2   // sensor still busy/uncalibrated after max wait

// Must be called once before AHT10_Read(). Performs power-up delay and,
// if needed, sends the calibration command. Requires I2C0_Init() and
// SysTick_Init() to have already been called.
uint8_t AHT10_Init(void);

// Triggers a measurement, waits for conversion, and returns the results.
// temperature_x10 = temperature in Celsius * 10 (one decimal digit), signed
// humidity_x10    = relative humidity in %      * 10 (one decimal digit)
uint8_t AHT10_Read(int32_t *temperature_x10, int32_t *humidity_x10);

// Returns the I2C0_ERR_* code (see I2C0.h) from the most recent I2C
// failure inside AHT10_Init()/AHT10_Read(). Only meaningful right after
// one of those returns AHT10_ERR_I2C -- lets the caller report exactly
// what went wrong (address NACK, data NACK, arbitration lost, timeout)
// instead of a generic "I2C error".
uint8_t AHT10_LastI2CError(void);

#endif