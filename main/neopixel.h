#include <stdbool.h>

#ifndef _NEOPIXEL_
#define _NEOPIXEL_

void espShow(uint8_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz);

#endif