#ifndef _ACCELEROMETER_
#define _ACCELEROMETER_
#include <stdio.h>

void initAccelerometer();
void readFromRegister(uint8_t register_location, int register_read_length, uint8_t *data);
void configureAccelerometer();
void getDeviceId(uint8_t *deviceId);
void getPowerControl(uint8_t *powerControl);
void writeToRegister(uint8_t register_location, int write_length, uint8_t *value);
void startMeasureMode();
void getQueueStatus(uint8_t *queue);
void getDataFormat(uint8_t *dataFormat);
void initializeDataFormat();
void enableWatermarkInterrupt();
void getInterruptStatus(uint8_t *status);
#endif