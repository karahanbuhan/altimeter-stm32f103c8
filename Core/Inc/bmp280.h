#ifndef BMP280_
#define BMP280_

#include <stdint.h>
#include <main.h>

float BMP280_ReadPressure(I2C_HandleTypeDef *hi2c1);
float BMP280_ReadTemperature(I2C_HandleTypeDef *hi2c1);
int BMP280_Init();

#endif
