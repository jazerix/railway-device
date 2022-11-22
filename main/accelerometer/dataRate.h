#ifndef _ACCELEROMETER_DATARATE_
#define _ACCELEROMETER_DATARATE_
#include <stdio.h>

#define DATA_RATE_25_HZ 0b00001000
#define DATA_RATE_50_HZ 0b00001001
#define DATA_RATE_100_HZ 0b00001010
#define DATA_RATE_200_HZ 0b00001011
#define DATA_RATE_400_HZ 0b00001100
#define DATA_RATE_800_HZ 0b00001101
#define DATA_RATE_1600_HZ 0b00001110
#define DATA_RATE_3200_HZ 0b00001111

void getDataRate(uint8_t *dateRate);
void printDataRate();
void setDataRate(uint8_t dataRate);

#endif