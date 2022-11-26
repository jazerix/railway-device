#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "accelerometer/measurement.h"
#include "esp_log.h"

#define TAG "WriteBuffer"
struct AccData *bufferA;
struct AccData *bufferB;

uint8_t currentBuffer = 0;
unsigned int index = 0;

void initWriteQueue()
{
    bufferA = calloc(1000, sizeof(struct AccData));
    bufferB = calloc(1000, sizeof(struct AccData));
    if (bufferA == NULL)
        ESP_LOGE(TAG, "Buffer a failed to be allocated - not enough heap memory");
    if (bufferB == NULL)
        ESP_LOGE(TAG, "Buffer a failed to be allocated - not enough heap memory");
    ESP_LOGI(TAG, "Initialized");
}

void saveValueToBuffer(struct AccData data)
{
    if (index == 999) //buffer full
    {
        // write to SD
        currentBuffer = !currentBuffer;
        ESP_LOGI(TAG, "Buffer full - switching to %d", currentBuffer);
        index = 0;
        return;
    }
    struct AccData *writeTo = currentBuffer == 0 ? bufferA : bufferB;
    writeTo[index] = data; 
    index++;
}

void deinitWriteQueue()
{
    free(bufferA);
    free(bufferB);
}
