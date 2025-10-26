/*
 * bmp180.h
 *
 * Simple C89 compliant driver for BMP180 sensor.
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include "main.h"

/**
 * @brief Initializes the BMP180 sensor.
 * This function checks if the device is ready and reads
 * the factory calibration coefficients.
 * @retval 1 (HAL_OK) if initialization is successful.
 * @retval 0 (HAL_ERROR) if the device is not found or calibration fails.
 */
uint8_t BMP180_Init(void);


/**
 * @brief Reads the raw temperature and pressure data and calculates
 * the true values.
 * @param[out] T: Pointer to a float to store the Temperature in Celsius (°C).
 * @param[out] P: Pointer to a float to store the Pressure in Pascal (Pa).
 */
void BMP180_Read_Temp_Pressure(float* T, float* P);


#endif /* INC_BMP180_H_ */
