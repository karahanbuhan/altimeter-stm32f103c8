#ifndef TM1637_
#define TM1637_

#include <stdint.h>

int TM1637_SetDisplay(uint32_t on);
int TM1637_ClearDisplay();
int TM1637_DisplayError(uint8_t error);
int TM1637_DisplayNumber(int16_t number, uint8_t show_colon);
int TM1637_DisplayDigits(uint8_t first_digit, uint8_t second_digit, uint8_t third_digit, uint8_t fourth_digit, uint8_t show_colon);
int TM1637_AllOn(void);

#endif
