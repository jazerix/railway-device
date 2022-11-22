#include <stdio.h>
#include <driver/i2c.h>
#include "esp_types.h"
#include "esp_log.h"
#include "accelerometer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "measurement.h"
#include "dataRate.h"
#include "calibration.h"

#define DATA_FORMAT_REGISTER 0x31
#define QUEUE_CTRL_REGISTER 0x38
#define QUEUE_REGISTER 0x39

#define SDA_GPIO 18
#define SCL_GPIO 19
#define TAG "Accelerometer"

i2c_config_t i2c_config;
TaskHandle_t xAccelerometerHandler = NULL;

uint32_t entries = 0;


void readSensorData()
{
    while(1)
    {
        struct AccDataDecimal data = getCurrentDataAsDecimal();
        printMeasurement(data);
        
    }
    vTaskDelete(NULL);
}


void initAccelerometer()
{
    ESP_LOGI(TAG, "Initializing");
    configureAccelerometer();
    initializeDataFormat();

    uint8_t queueConfig = 0b00000000; // fifo disabled// 0b01011111;
    writeToRegister(QUEUE_CTRL_REGISTER, 1, &queueConfig);
    setDataRate(DATA_RATE_800_HZ);
    printDataRate();

    startCalibration();

    queueConfig = 0b01011111;
    writeToRegister(QUEUE_CTRL_REGISTER, 1, &queueConfig);
    startMeasureMode();
    xTaskCreatePinnedToCore(readSensorData, "read_acc", 2000, NULL, 3, &xAccelerometerHandler, 1);

    

    /*vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "INTR PIN STATUS: %d", gpio_get_level(INTR_PIN));
    getQueueStatus(&queueStatus);
    ESP_LOGI(TAG, "Current queue status: %x", queueStatus);*/
    // xTaskCreatePinnedToCore(getCurrentData, "read_acc", 2000, NULL, 3, &xAccelerometerHandler, 1);

    /*uint8_t dataFormat = 0;
    getDataFormat(&dataFormat);
    ESP_LOGI(TAG, "Current data format: %x", dataFormat);

    uint8_t deviceId[1];

    getDeviceId(&deviceId);
    ESP_LOGI(TAG, "Reading device id: %x", deviceId[0]);

    uint8_t powerConfig = 0;
    getPowerControl(&powerConfig);
    ESP_LOGI(TAG, "Power: %x", powerConfig);


    getQueueStatus(&queueStatus);
    ESP_LOGI(TAG, "Current queue status: %x", queueStatus);



    getPowerControl(&powerConfig);
    ESP_LOGI(TAG, "Power: %x", powerConfig);

    */

    /*
     startMeasureMode();

     uint8_t queueStatus = 0;
     getQueueStatus(&queueStatus);
     ESP_LOGI(TAG, "Current queue status: %x", queueStatus);

     vTaskDelay(10 / portTICK_PERIOD_MS);

     getQueueStatus(&queueStatus);
     ESP_LOGI(TAG, "Current queue status: %x", queueStatus);

     // uint8_t data[6];
     // while(1)
     xTaskCreatePinnedToCore(getCurrentData, "read_acc", 2000, NULL, 3, &xAccelerometerHandler, 1);

     getQueueStatus(&queueStatus);
     ESP_LOGI(TAG, "Current queue status: %x", queueStatus);*/
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



void getQueueStatus(uint8_t *queue)
{
    readFromRegister(QUEUE_REGISTER, 1, queue);
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
