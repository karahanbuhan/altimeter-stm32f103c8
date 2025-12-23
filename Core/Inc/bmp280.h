#ifndef BMP280_
#define BMP280_

#include <stdint.h>
#include <main.h>

float BMP280_Read_Pressure(I2C_HandleTypeDef *hi2c1);
float BMP280_Read_Temperature(I2C_HandleTypeDef *hi2c1);
int BMP280_Init();

#endif
