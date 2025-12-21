#include "main.h"
#include <stdint.h>

float BMP280_Read_Pressure() {
	return 980.15f;
}

int BMP280_Init() {
	if (HAL_I2C_IsDeviceReady(&hi2c1, 0x76 << 1, 3, 100) == HAL_OK) {
		/* Let's try reset to recover in case peripheral was reconnected
		 * This recover wouldn't be required in case SDL was disconnected
		 * however if the clock were to disconnected I2C would crash thus
		 * require de-initilization even if the connection was re-established
		 * so we need to reset it */
		HAL_I2C_DeInit(&hi2c1);
		MX_I2C1_Init();
		return 0;
	}
	return 1;
}
