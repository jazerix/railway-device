#ifndef _WRITEBUFFER_
#define _WRITEBUFFER_
#include "accelerometer/measurement.h"
void initWriteBuffer();
void saveValueToBuffer(struct AccData data);
void deinitWriteBuffer();
#endif