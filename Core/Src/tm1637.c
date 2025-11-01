#include <stdint.h>
#include "main.h"

static void us_delay(uint32_t us);
static void start(void);
static void write_byte(uint8_t data_byte);
static int acknowledge(void);
static void terminate(void);


int TM1637_SetDisplay(uint32_t on) {
	/* 10001000 or 0x88 (Display On) */
	start();
	if (on) {
	 	write_byte(0b10001000);
	} else {
		write_byte(0b10000000);
	}
	if (!acknowledge()) {
		return 0;
	}
	terminate();
	return 1;
}

int TM1637_AllOn(void) {
	/* Pin 4 is DIO, 5 is CLCK */
	/* 01000000 = 0x40: Data Command Set, starting with this command */
	start();
	write_byte(0b01000000);
	if (!acknowledge()) {
		return 0;
	}
	terminate();

	/* 11000000 = 0xC0 Address Command Set, we will set the segments later */
	start();
	write_byte(0b11000000);
	if (!acknowledge()) {
		return 0;
	}

	/* Send x4 0xFFs as addresses to set all segments on */
	for (int i = 0; i < 4; i++) {
		write_byte(0xff);
		if (!acknowledge()) {
			return 0;
		}
	}
	terminate();

	return 1;
}

static void start(void) {
	/* Pin 4 is DIO, 5 is CLCK */
	/* Start by all HIGH to wake TM1637 up */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	us_delay(5);

	/* We set DIO low to start */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	us_delay(5);
}

static void write_byte(uint8_t data_byte) {
	/* Sending in LSB (bit 0) first fashion */
	for (int i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		/* If i=6, 01000000 becomes 00000001 after shift and 1 & 1 is 1, so 7th bit is 1 */
		if ((data_byte >> i) & 1) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
		us_delay(5);
	}
}

static int acknowledge(void) {
	int ack;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	us_delay(5);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	/* TM1637 sets DIO to 0 if acknowledge successful */
	ack = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET;

	us_delay(5);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	us_delay(5);

	return ack;
}

static void terminate(void) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	us_delay(5);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	us_delay(5);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); /* Stops */
	us_delay(5);
}

static void us_delay(uint32_t us) {
	/* Get the system clock frequency */
	uint32_t cycles_per_us = (HAL_RCC_GetHCLKFreq() / 1000000);

	/* Calculate the number of cycles to wait */
	uint32_t start_cycle = DWT->CYCCNT;
	uint32_t wait_cycles = us * cycles_per_us;

	/* Loop until the required cycles have passed */
	while ((DWT->CYCCNT - start_cycle) < wait_cycles) {
		/* Do nothing here, wait */
	}
}
