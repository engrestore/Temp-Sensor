// AHT10.c
#include "I2C0.h"
#include "SysTick.h"
#include "AHT10.h"

// AHT10 command bytes (from AHT10 datasheet)
#define AHT10_CMD_CALIBRATE_0   0xE1
#define AHT10_CMD_CALIBRATE_1   0x08
#define AHT10_CMD_CALIBRATE_2   0x00

#define AHT10_CMD_TRIGGER_0     0xAC
#define AHT10_CMD_TRIGGER_1     0x33
#define AHT10_CMD_TRIGGER_2     0x00

#define AHT10_STATUS_BUSY_BIT   0x80   // bit7 of status byte
#define AHT10_STATUS_CAL_BIT    0x08   // bit3 of status byte

// Remembers the underlying I2C0_ERR_* code from the most recent I2C
// failure, so callers can report exactly what went wrong via
// AHT10_LastI2CError() instead of just seeing generic AHT10_ERR_I2C.
static uint8_t s_lastI2CErr = I2C0_OK;

uint8_t AHT10_LastI2CError(void)
{
    return s_lastI2CErr;
}

uint8_t AHT10_Init(void)
{
    uint8_t cmd[3];
    uint8_t status;
    uint8_t err;
    uint8_t tries;

    // AHT10 needs >= 40 ms after power-up before the first command
    SysTick_Wait1ms(40);

    // Check calibration status: read 1 status byte
    err = I2C0_Read(AHT10_I2C_ADDR, &status, 1);
    if (err != I2C0_OK) { s_lastI2CErr = err; return AHT10_ERR_I2C; }

    if ((status & AHT10_STATUS_CAL_BIT) == 0) {
        cmd[0] = AHT10_CMD_CALIBRATE_0;
        cmd[1] = AHT10_CMD_CALIBRATE_1;
        cmd[2] = AHT10_CMD_CALIBRATE_2;
        err = I2C0_Write(AHT10_I2C_ADDR, cmd, 3);
        if (err != I2C0_OK) { s_lastI2CErr = err; return AHT10_ERR_I2C; }

        // Poll until the sensor actually reports calibrated instead of
        // trusting a fixed delay -- some AHT10 clones take longer than
        // others to finish calibrating.
        for (tries = 0; tries < 10; tries++) {
            SysTick_Wait1ms(10);
            err = I2C0_Read(AHT10_I2C_ADDR, &status, 1);
            if (err != I2C0_OK) { s_lastI2CErr = err; return AHT10_ERR_I2C; }
            if (status & AHT10_STATUS_CAL_BIT) break;
        }
        if ((status & AHT10_STATUS_CAL_BIT) == 0) return AHT10_ERR_BUSY;
    }

    return AHT10_OK;
}

uint8_t AHT10_Read(int32_t *temperature_x10, int32_t *humidity_x10)
{
    uint8_t cmd[3];
    uint8_t raw[6];
    uint8_t err;
    uint8_t tries;
    uint32_t humidity_raw;
    uint32_t temp_raw;

    cmd[0] = AHT10_CMD_TRIGGER_0;
    cmd[1] = AHT10_CMD_TRIGGER_1;
    cmd[2] = AHT10_CMD_TRIGGER_2;
    err = I2C0_Write(AHT10_I2C_ADDR, cmd, 3);
    if (err != I2C0_OK) { s_lastI2CErr = err; return AHT10_ERR_I2C; }

    // Typical conversion time is ~75-80 ms
    SysTick_Wait1ms(80);

    // Poll status bit (bit7) in case conversion isn't finished yet
    for (tries = 0; tries < 5; tries++) {
        err = I2C0_Read(AHT10_I2C_ADDR, raw, 6);
        if (err != I2C0_OK) { s_lastI2CErr = err; return AHT10_ERR_I2C; }
        if ((raw[0] & AHT10_STATUS_BUSY_BIT) == 0) break;
        SysTick_Wait1ms(20);
    }
    if (raw[0] & AHT10_STATUS_BUSY_BIT) return AHT10_ERR_BUSY;

    // raw[0] = status
    // raw[1..2] + top nibble of raw[3] = 20-bit humidity
    // bottom nibble of raw[3] + raw[4..5] = 20-bit temperature
    humidity_raw = ((uint32_t)raw[1] << 12) | ((uint32_t)raw[2] << 4) | (raw[3] >> 4);
    temp_raw     = (((uint32_t)raw[3] & 0x0F) << 16) | ((uint32_t)raw[4] << 8) | raw[5];

    // humidity% *10  = raw / 2^20 * 100 * 10 = raw * 1000 / 1048576
    *humidity_x10 = (int32_t)(((int64_t)humidity_raw * 1000) / 1048576);

    // temp C *10 = (raw / 2^20 * 200 - 50) * 10 = raw*2000/1048576 - 500
    *temperature_x10 = (int32_t)(((int64_t)temp_raw * 2000) / 1048576) - 500;

    return AHT10_OK;
}