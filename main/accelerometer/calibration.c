#include <stdio.h>
#include "communication.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "measurement.h"
#include "esp_log.h"
#include "queue.h"
#include "../state.h"
#include "offset.h"
#include "../status.h"

#define TAG "Accelerometer Calibration"
#define TARGET_SAMPLES 500
#define OFFSET_X_REGISTER 0x1e
#define OFFSET_Y_REGISTER 0x1f
#define OFFSET_Z_REGISTER 0x20

int averageSumX = 0;
int averageSumY = 0;
int averageSumZ = 0;

void setOffsetRegister(uint8_t registerLocation, int8_t offset)
{
    offset = -(offset / 4);
    uint8_t conversion = offset;
    writeToRegister(registerLocation, 1, &conversion);
}

void resetOffset()
{
    uint8_t offset = 0x00;
    writeToRegister(OFFSET_X_REGISTER, 1, &offset);
    writeToRegister(OFFSET_Y_REGISTER, 1, &offset);
    writeToRegister(OFFSET_Z_REGISTER, 1, &offset); 
}

struct AccData average(int samples, bool printResults)
{
    averageSumX = 0;
    averageSumY = 0;
    averageSumZ = 0;
    for(size_t i = 0; i < samples; i++)
    {
        struct AccData data = getCurrentData();
        averageSumX += data.x;
        averageSumY += data.y;
        averageSumZ += data.z;
    }

    struct AccData data = {
        .x = averageSumX / samples,
        .y = averageSumY / samples,
        .z = averageSumZ / samples
    };

    if (printResults)
        ESP_LOGI(TAG, "Average: X%d Y%d Z%d", data.x, data.y, data.z);    

    return data;
}


void startCalibration()
{
    ESP_LOGI(TAG, "Calibrating sensor");
    setStatus(LED_CALIBRATING);
    resetOffset();
    disableQueue();
    startMeasureMode();
    struct AccData data = average(TARGET_SAMPLES, true);

    int8_t offsetX = data.x;
    int8_t offsetY = data.y;
    int8_t offsetZ = data.z - 256; // default is 1g for the Z axis to account for gravity

    ESP_LOGI(TAG, "Offset: X%d, Y%d, Z%d", offsetX, offsetY, offsetZ);
    setOffsetRegister(OFFSET_X_REGISTER, offsetX);
    setOffsetRegister(OFFSET_Y_REGISTER, offsetY);
    setOffsetRegister(OFFSET_Z_REGISTER, offsetZ);
    stopMeasureMode();

    struct OffsetData tempOffsets = {
        .x = offsetX,
        .y = offsetY,
        .z = offsetX
    };
    offsets = tempOffsets;
    ESP_LOGI(TAG, "Sensor calibrated");
}
