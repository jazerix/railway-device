#ifndef _ACCELEROMETER_MEASUREMENT_
#define _ACCELEROMETER_MEASUREMENT_
#include <stdio.h>
struct AccData
{
    int16_t x;
    int16_t y;
    int16_t z;
    int64_t time;
};

struct AccDataDecimal
{
    double x;
    double y;
    double z;
};

struct AccData getCurrentData();
struct AccDataDecimal getCurrentDataAsDecimal();
struct AccDataDecimal accConvertToDecimal(struct AccData accData);
void startMeasureMode();
void stopMeasureMode();
void printMeasurement(struct AccDataDecimal accDataDecimal);
#endif