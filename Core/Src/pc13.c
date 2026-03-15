#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* State of the blinking machine */
enum state {WAITING, READY, BLINKING} state;

/* This is used for finite state machine logic and to store when the last handle call was */
uint32_t previous_time = 0;

uint32_t start_blinking_time = 0;
/* Time for when blinking will stop looks like this, it is never ending until error is 0
 * Formula for total period is: 600 * (current_error) - 200
 * Er(2) = . .    . .
 * Timing: 200ms high, 200ms low, 200ms high, 400ms sleep for n=2
 * Er(3) = . . .      . . . */

/* Save error in case it changes midway while finishing a one complete cycle */
uint8_t current_error = 0;

static void start_blinking(uint32_t current_time, uint8_t error);
static void reset();

void PC13_DisplayError(uint32_t current_time, uint8_t error) {
	/* Reset machine and do nothing if error is 0 */
	if (error == 0) {
		reset();
		return;
	}

	if (previous_time == 0 && state == WAITING) {
		previous_time = current_time;
		/* WAITING is used just in case previous time is not set yet */
		state = READY;
		return;
	}

	/* Start blinking if second call */
	if (start_blinking_time == 0 && state == READY) {
		previous_time = current_time;
		start_blinking(current_time, error); /* Sets the start blinking time */
		return;
	}

	/* Abort if not in blinking state */
	if (state != BLINKING) {
		return;
	}

	uint32_t diff = current_time - previous_time;
	uint32_t t = (600 * current_error) - 200; /* Total period of one blink cycle */
	uint32_t bt = (400 * current_error) - 200; /* Blink period of the cycle */

	/* M = tick (mod t): Gets us the time value in range of [0, t] to handle blink/sleep states */
	/* M > bt: M is out of the blink period so in sleep period of the cycle, waiting for next cycle */
	/* M % 400 <= 200: High and low cycles are 200ms+200ms. First 200ms is high */
	uint32_t m = current_time % t;
	bool in_sleep_period = m > bt;
	bool in_blink_period = m % 400 <= 200 && !(in_sleep_period);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, in_blink_period ? GPIO_PIN_SET : GPIO_PIN_RESET);

	/* TODO: Remove unnecessary time variables and other variables */

	/* Store previous time for next call */
	previous_time = current_time;
}

static void start_blinking(uint32_t current_time, uint8_t error) {
	if (state == READY) {
		start_blinking_time = current_time;
		current_error = error;
		state = BLINKING;
	} else {
		/* Else condition shall never occur without hardware failure, though reset the machine just in case */
		reset();
	}
}

static void reset() {
	previous_time = 0;
	start_blinking_time = 0;
	current_error = 0;
	state = WAITING;
}
