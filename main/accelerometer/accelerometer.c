#include <stdio.h>
#include <driver/i2c.h>
#include "esp_types.h"
#include "esp_log.h"
#include "accelerometer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "stdbool.h"

#include "measurement.h"
#include "dataRate.h"
#include "calibration.h"
#include "queue.h"
#include "communication.h"
#include "../writeBuffer.h"

#define DATA_FORMAT_REGISTER 0x31

#define SDA_GPIO 18
#define SCL_GPIO 19
#define TAG "Accelerometer"

i2c_config_t i2c_config;
TaskHandle_t xAccelerometerHandler = NULL;

uint32_t entries = 0;
bool shouldStop = false;

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

void readSensorData()
{
    while (true)
    {
        if (shouldStop == true)
            break;
        struct AccData data = getCurrentData();
        saveValueToBuffer(data);
        /* ESP_LOGI(TAG, "Measurement: X%dY%dZ%d Time: %d", data.x, data.y, data.z, data.time);
        deinitWriteBuffer();
        break;*/
    }
    xAccelerometerHandler = NULL;
    shouldStop = false;
    vTaskDelete(NULL);
}

void initAccelerometer()
{
    ESP_LOGI(TAG, "Initializing");
    configureAccelerometer();
    initializeDataFormat();
    setDataRate(DATA_RATE_800_HZ);
    printDataRate();
}

void startMeasurement()
{
    setQueueMode(QUEUE_FIFO);
    startMeasureMode();
    shouldStop = false;
    xTaskCreatePinnedToCore(readSensorData, "read_acc", 2000, NULL, 3, &xAccelerometerHandler, 1);

    // xTaskCreate(monitor_queue, "monitor_queue", 2000, NULL, 3, NULL);
}

void stopMeasurement()
{
    shouldStop = true;
    while(xAccelerometerHandler != NULL)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "%p", &xAccelerometerHandler);
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
