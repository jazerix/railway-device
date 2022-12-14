#include <stdio.h>
#include <driver/i2c.h>
#include "esp_types.h"
#include "esp_log.h"
#include "accelerometer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "FreeRTOSConfig.h"
#include "stdbool.h"
#include "../state.h"
#include "measurement.h"
#include "dataRate.h"
#include "queue.h"
#include "communication.h"
#include "../writeBuffer.h"

#define PRINT_SAMPLES true
#define DATA_FORMAT_REGISTER 0x31

#define SDA_GPIO 18
#define SCL_GPIO 19
#define TAG "Accelerometer"

i2c_config_t i2c_config;
TaskHandle_t xAccelerometerHandler = NULL;
TaskHandle_t xAccelerometerHandlerSecondary = NULL;

bool shouldStopPrimary = false;
bool shouldStopSecondary = false;
int samples = 0;

void monitor_queue()
{
    while (true)
    {
        uint8_t queue = 0;
        getQueueStatus(&queue);
        ESP_LOGI(TAG, "Queue: %d", queue);
    }
    vTaskDelete(NULL);
}

int counter = 1;
void samplesPerSec()
{
    while(true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "[%03d] Samples last second: %d", counter, samples);
        samples = 0;
        counter++;
    }
}

void readSensorData()
{
    struct AccData data = getCurrentData();
    saveValueToBuffer(data);
    if (PRINT_SAMPLES)
        samples++;
}

void readFromPrimary()
{
    while (true)
    {
        if (shouldStopPrimary == true)
            break;
        readSensorData();
        samplesPrimary++;
    }
    xAccelerometerHandler = NULL;
    shouldStopPrimary = false;
    vTaskDelete(NULL);
}

void readFromSecondary()
{
    while (true)
    {
        if (shouldStopSecondary == true)
            break;
        readSensorData();
        samplesSecondary++;
    }
    xAccelerometerHandlerSecondary = NULL;
    shouldStopSecondary = false;
    vTaskDelete(NULL);
}

void initAccelerometer()
{
    ESP_LOGI(TAG, "Initializing");
    configureAccelerometer();
    initializeDataFormat();
    setDataRate(DATA_RATE_1600_HZ);
    printDataRate();
}

void startMeasurement()
{
    setQueueMode(QUEUE_FIFO);
    startMeasureMode();
    shouldStopPrimary = false;
    shouldStopSecondary = false;
    xTaskCreatePinnedToCore(readFromPrimary, "read_acc", 2000, NULL, configMAX_PRIORITIES, &xAccelerometerHandler, 1);
    xTaskCreatePinnedToCore(readFromSecondary, "read_acc_second", 2000, NULL, configMAX_PRIORITIES, &xAccelerometerHandlerSecondary, 0);
    if (PRINT_SAMPLES)
        xTaskCreate(samplesPerSec, "samples_per_sec", 2000, NULL, 1, NULL);

    // xTaskCreate(monitor_queue, "monitor_queue", 2000, NULL, 3, NULL);
}

void stopMeasurement()
{
    shouldStopPrimary = true;
    shouldStopSecondary = true;
    while(xAccelerometerHandler != NULL || xAccelerometerHandlerSecondary != NULL)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Waiting for handlers to stop: %p %p", &xAccelerometerHandler, &xAccelerometerHandlerSecondary);
    }
    stopMeasureMode();
}

void configureAccelerometer()
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000 // 100khz
    };
    i2c_config = config;

    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void getDeviceId(uint8_t *deviceId)
{
    readFromRegister(0x00, 1, deviceId);
}

void getDataFormat(uint8_t *dataFormat)
{
    readFromRegister(DATA_FORMAT_REGISTER, 1, dataFormat);
}

void initializeDataFormat()
{
    uint8_t dataToWrite = 0b00001000;
    writeToRegister(DATA_FORMAT_REGISTER, 1, &dataToWrite); // 2g range + full resolution
}
