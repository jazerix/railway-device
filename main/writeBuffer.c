#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "accelerometer/measurement.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdcard.h"
#include <inttypes.h>

#define TAG "WriteBuffer"
#define BUFFER_SIZE 1000

struct AccData *bufferA;
struct AccData *bufferB;

uint8_t currentBuffer = 0;
unsigned int index = 0;

TaskHandle_t savingTask = NULL;

void saveBufferTask(struct AccData *buffer)
{
    writeSensorData(buffer);
    savingTask = NULL;
    vTaskDelete(NULL);
}

void initWriteBuffer()
{
    bufferA = calloc(BUFFER_SIZE, sizeof(struct AccData));
    bufferB = calloc(BUFFER_SIZE, sizeof(struct AccData));
    /*if (bufferA == NULL)
        ESP_LOGE(TAG, "Buffer a failed to be allocated - not enough heap memory");
    if (bufferB == NULL)
        ESP_LOGE(TAG, "Buffer a failed to be allocated - not enough heap memory");*/
    ESP_LOGI(TAG, "Initialized");
}

void saveBuffer()
{
    struct AccData *writeFrom = currentBuffer == 0 ? bufferA : bufferB;
    while (savingTask != NULL)
    {
        ESP_LOGW(TAG, "Saving buffer is taking too long!");
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    savingTask = xTaskCreate(saveBufferTask, "saveBuffer", 2000, writeFrom, 25, NULL);
    currentBuffer = !currentBuffer;
}

void saveValueToBuffer(struct AccData data)
{
    if (index == BUFFER_SIZE) // buffer full
    {
        saveBuffer();
        ESP_LOGI(TAG, "Buffer full - switching to %d", currentBuffer);
        index = 0;
        return;
    }
    struct AccData *writeTo = currentBuffer == 0 ? bufferA : bufferB;
    writeTo[index] = data;
    index++;
}

/**
 * Better solution would be to use something like memset but this is pretty explanatory for now
 */
void padRemaining()
{
    for(int i = index; i < BUFFER_SIZE; i++)
    {
        struct AccData data = {
                .x = 0,
                .y = 0,
                .z = 0,
                .time = 0
        };
        saveValueToBuffer(data);
    }
}


void deinitWriteBuffer()
{
    if (index != 0) {
        padRemaining();
        saveBuffer();
    }
    index = 0;
    free(bufferA);
    free(bufferB);
}
