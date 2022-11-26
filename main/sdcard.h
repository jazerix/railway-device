#ifndef _SDCARD_
#define _SDCARD_
#include "accelerometer/measurement.h"
void initSd();
void createFile(int recordingId);
void closeCurrentFile();
void writeSensorData(struct AccData *sensorData);
#endif