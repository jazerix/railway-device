#include <stdio.h>
#include "accelerometer.h"
#include "queue.h"
#include "esp_log.h"

#define TAG "Accelerometer"
#define QUEUE_SIZE 32
#define QUEUE_TRIGGER 0b0
#define QUEUE_CTRL_REGISTER 0x38
#define QUEUE_REGISTER 0x39


void disableQueue()
{
    uint8_t queueConfig = 0b00000000; // complete disable queue
    writeToRegister(QUEUE_CTRL_REGISTER, 1, &queueConfig);
}

void setQueueMode(uint8_t mode)
{
    uint8_t size = QUEUE_SIZE;
    if (size > 31)
    {
        ESP_LOGE(TAG, "Max queue size is 32 (0-index), %d was set.", QUEUE_SIZE);
        return;
    }

    uint8_t queueConfig = mode << 6 | QUEUE_TRIGGER << 5 | size;    
    writeToRegister(QUEUE_CTRL_REGISTER, 1, &queueConfig);
}

void getQueueStatus(uint8_t *queue)
{
    readFromRegister(QUEUE_REGISTER, 1, queue);
}