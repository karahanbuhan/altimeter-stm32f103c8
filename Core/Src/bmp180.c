/*
 * bmp180.c
 *
 * Simple driver for BMP180 sensor.
 */

#include "bmp180.h"
#include <math.h>

/* I2C Handle defined in main.c */
extern I2C_HandleTypeDef hi2c1;

/* Sensor's I2C Address (0x76 shifted left by 1) */
#define BMP180_DEVICE_ADDRESS (0x76 << 1)

/* Calibration registers */
#define BMP180_CALIB_REG_START 0xAA

/* Control and Data registers */
#define BMP180_CONTROL_REG 0xF4
#define BMP180_DATA_REG 0xF6

/* Control register commands */
#define BMP180_READ_TEMP_CMD 0x2E
#define BMP180_READ_PRESSURE_CMD 0x34 /* OSS = 0 */

/* Static variables to store calibration data */
static int16_t AC1, AC2, AC3;
static uint16_t AC4, AC5, AC6;
static int16_t B1, B2, MB, MC, MD;
static long B5; /* This is calculated in temp reading and used in pressure reading */

/**
 * @brief Reads the factory calibration coefficients from the sensor's EEPROM.
 */
uint8_t BMP180_Init(void) {
    uint8_t calibData[22];

    /* 1. Check if the device is ready */
    if (HAL_I2C_IsDeviceReady(&hi2c1, BMP180_DEVICE_ADDRESS, 1, 1000) != HAL_OK) {
        return 0; /* Device not found */
    }

    /* 2. Read all 22 bytes of calibration data */
    if (HAL_I2C_Mem_Read(&hi2c1, BMP180_DEVICE_ADDRESS, BMP180_CALIB_REG_START, I2C_MEMADD_SIZE_8BIT, calibData, 22, 1000) != HAL_OK) {
        return 0; /* Calibration data read error */
    }

    /* 3. Parse the calibration data (convert from 2x 8-bit to 1x 16-bit) */
    AC1 = (calibData[0] << 8) | calibData[1];
    AC2 = (calibData[2] << 8) | calibData[3];
    AC3 = (calibData[4] << 8) | calibData[5];
    AC4 = (calibData[6] << 8) | calibData[7];
    AC5 = (calibData[8] << 8) | calibData[9];
    AC6 = (calibData[10] << 8) | calibData[11];
    B1 = (calibData[12] << 8) | calibData[13];
    B2 = (calibData[14] << 8) | calibData[15];
    MB = (calibData[16] << 8) | calibData[17];
    MC = (calibData[18] << 8) | calibData[19];
    MD = (calibData[20] << 8) | calibData[21];

    return 1; /* Initialization successful */
}

/**
 * @brief Reads the uncompensated (raw) temperature value.
 */
static long readRawTemperature(void) {
    uint8_t cmd;
    uint8_t data[2];

    cmd = BMP180_READ_TEMP_CMD;

    HAL_I2C_Mem_Write(&hi2c1, BMP180_DEVICE_ADDRESS, BMP180_CONTROL_REG, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
    HAL_Delay(5); /* Wait 5ms for conversion */
    HAL_I2C_Mem_Read(&hi2c1, BMP180_DEVICE_ADDRESS, BMP180_DATA_REG, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);

    return (data[0] << 8) | data[1];
}

/**
 * @brief Reads the uncompensated (raw) pressure value (Oversampling=0).
 */
static long readRawPressure(void) {
    uint8_t cmd;
    uint8_t data[3];

    cmd = BMP180_READ_PRESSURE_CMD; /* OSS = 0 */

    HAL_I2C_Mem_Write(&hi2c1, BMP180_DEVICE_ADDRESS, BMP180_CONTROL_REG, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
    HAL_Delay(5); /* Wait 5ms for conversion */
    HAL_I2C_Mem_Read(&hi2c1, BMP180_DEVICE_ADDRESS, BMP180_DATA_REG, I2C_MEMADD_SIZE_8BIT, data, 3, 1000);

    return ((data[0] << 16) + (data[1] << 8) + data[2]) >> 8;
}

/**
 * @brief Main function to read and calculate true values.
 */
void BMP180_Read_Temp_Pressure(float* T, float* P) {
    long UT, UP;
    long X1, X2, X3;
    long B3, B4, B6, B7;

    /* --- Temperature Calculation --- */
    UT = readRawTemperature();
    X1 = (UT - AC6) * AC5 / 32768;
    X2 = (long)MC * 2048 / (X1 + MD);
    B5 = X1 + X2; /* B5 is stored globally for pressure calculation */
    *T = (float)((B5 + 8) / 16) / 10.0f; /* Final Temperature in degC */

    /* --- Pressure Calculation --- */
    UP = readRawPressure();
    B6 = B5 - 4000;
    X1 = (B2 * (B6 * B6 / 4096)) / 2048;
    X2 = AC2 * B6 / 2048;
    X3 = X1 + X2;
    B3 = (((AC1 * 4 + X3)) + 2) / 4;
    X1 = AC3 * B6 / 8192;
    X2 = (B1 * (B6 * B6 / 4096)) / 65536;
    X3 = ((X1 + X2) + 2) / 4;
    B4 = AC4 * (unsigned long)(X3 + 32768) / 32768;
    B7 = ((unsigned long)UP - B3) * 50000;

    if (B7 < 0x80000000) {
        *P = (float)((B7 * 2) / B4);
    } else {
        *P = (float)((B7 / B4) * 2);
    }

    X1 = (*P / 256) * (*P / 256);
    X1 = (X1 * 3038) / 65536;
    X2 = (-7357 * *P) / 65536;
    *P = *P + (X1 + X2 + 3791) / 16; /* Final Pressure in Pascal */
}
