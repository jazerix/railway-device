#ifndef _ACCELEROMETER_QUEUE_
#define _ACCELEROMETER_QUEUE_
#include <stdint.h>
#define QUEUE_BYPASS 0b00
#define QUEUE_FIFO 0b01
#define QUEUE_STREAM 0b10
#define QUEUE_TRIGGER 0b11

void disableQueue();
void setQueueMode(uint8_t mode);
void getQueueStatus(uint8_t *queue);
#endif