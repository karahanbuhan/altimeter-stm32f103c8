#include "main.h"
#include <stdint.h>

/* State of the blinking machine */
enum state {WAITING, READY, BLINKING} state;

/* This is used for finite state machine logic and to store when the last handle call was */
uint32_t previous_time = 0;

uint32_t start_blinking_time = 0;
/* Time for when blinking will stop looks like this
 * Formula is: (error)(interval) + (error - 1)(interval) + (2)(interval)(error - 1)
 * Er(2) = . .    . .
 * Er(3) = . . .      . . .      . . . */

/* Save error in case it changes midway while finishing a one complete cycle */
uint8_t current_error = 0;

void PC13_Handle_Error(uint32_t current_time, uint8_t error) {
	if (previous_time == 0 && state == WAITING) {
		previous_time = current_time;
		/* WAITING is used just in case previous time is not set yet */
		state = READY;
		return;
	}

	/* Start blinking if second call */
	if (start_blinking_time == 0 && state == READY) {
		previous_time = current_time;
		start_blinking(current_time, error);
		return;
	}

	/* Abort if not in blinking state */
	if (state != BLINKING) {
		return;
	}

	uint32_t diff = current_time - previous_time;


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
		previous_time = 0;
		start_blinking_time = 0;
		current_error = 0;
		state = WAITING;
	}
}
