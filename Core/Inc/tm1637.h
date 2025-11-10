#ifndef TM1637_
#define TM1637_

#include <stdint.h>

int TM1637_SetDisplay(uint32_t on);
int TM1637_DisplayDigits(uint8_t first_digit, uint8_t second_digit, uint8_t third_digit, uint8_t fourth_digit, uint8_t show_colon);
int TM1637_AllOn(void);

#endif
