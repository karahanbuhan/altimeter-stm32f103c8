#include "main.h"
#include <stdint.h>

// I2C Address (0x76 shifted left by 1)
#define BMP280_ADDR (0x76 << 1)

// Calibration Data Globals (Required for math)
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int32_t t_fine = 0; // Shared thermal variable

int BMP280_Init(I2C_HandleTypeDef *hi2c1) {
	// ERROR 1: Device not found (Wrong Address or Wiring)
	if (HAL_I2C_IsDeviceReady(hi2c1, BMP280_ADDR, 5, 100) != HAL_OK) {
		return 1;
	}

	// ERROR 2: Failed to read Calibration Data
	uint8_t calib[24];
	if (HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0x88, 1, calib, 24, 100)
			!= HAL_OK) {
		return 2;
	}

	// Assign data (same as before)
	dig_T1 = calib[0] | (calib[1] << 8);
	dig_T2 = calib[2] | (calib[3] << 8);
	dig_T3 = calib[4] | (calib[5] << 8);
	dig_P1 = calib[6] | (calib[7] << 8);
	dig_P2 = calib[8] | (calib[9] << 8);
	dig_P3 = calib[10] | (calib[11] << 8);
	dig_P4 = calib[12] | (calib[13] << 8);
	dig_P5 = calib[14] | (calib[15] << 8);
	dig_P6 = calib[16] | (calib[17] << 8);
	dig_P7 = calib[18] | (calib[19] << 8);
	dig_P8 = calib[20] | (calib[21] << 8);
	dig_P9 = calib[22] | (calib[23] << 8);

	// ERROR 3: Failed to write Configuration
	uint8_t config = 0x27; // Normal mode, temp/pres oversampling x1
	if (HAL_I2C_Mem_Write(hi2c1, BMP280_ADDR, 0xF4, 1, &config, 1, 100)
			!= HAL_OK) {
		return 3;
	}

	// SUCCESS: Returns 0
	return 0;
}

// Returns Temperature in Celsius
float BMP280_Read_Temperature(I2C_HandleTypeDef *hi2c1) {
	uint8_t raw[3];
	// Read 0xFA, 0xFB, 0xFC
	HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0xFA, 1, raw, 3, 100);

	int32_t adc_T = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);

	// Standard Bosch Compensation
	int32_t var1, var2;
	var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2))
			>> 11;
	var2 = (((((adc_T >> 4) - ((int32_t) dig_T1))
			* ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3))
			>> 14;
	t_fine = var1 + var2; // Update global t_fine for pressure calc

	return (float) ((t_fine * 5 + 128) >> 8) / 100.0f;
}

// Returns Pressure in Pascals (Divide by 100 for hPa)
float BMP280_Read_Pressure(I2C_HandleTypeDef *hi2c1) {
    if (t_fine == 0) {
        BMP280_Read_Temperature(hi2c1);
    }

    uint8_t raw[3];
    if (HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0xF7, 1, raw, 3, 100) != HAL_OK) {
        BMP280_Init(hi2c1);
        return 0.0f;
    }

    int32_t adc_P = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);

    if (adc_P == 0x80000) {
        BMP280_Init(hi2c1);
        HAL_Delay(10);

        HAL_I2C_Mem_Read(hi2c1, BMP280_ADDR, 0xF7, 1, raw, 3, 100);
        adc_P = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);

        if (adc_P == 0x80000) return 0.0f;
    }

    int64_t var1, var2, p;
    var1 = ((int64_t) t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) dig_P6;
    var2 = var2 + ((var1 * (int64_t) dig_P5) << 17);
    var2 = var2 + (((int64_t) dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t) dig_P3) >> 8)
            + ((var1 * (int64_t) dig_P2) << 12);
    var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t) dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t) dig_P7) << 4);

    float final_pressure = (float) p / 25600.0f;

    if (final_pressure < 700.0f && final_pressure > 0.1f) {
         BMP280_Init(hi2c1);
    }

    return final_pressure;
}
