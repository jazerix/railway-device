#include <stdio.h>
#include "communication.h"
#include "dataRate.h"
#include "esp_log.h"

#define TAG "Accelerometer"

#define BW_DATARATE_REGISTER 0x2c

void getDataRate(uint8_t *dateRate)
{
    readFromRegister(BW_DATARATE_REGISTER, 1, dateRate);
}

void setDataRate(uint8_t dataRate)
{
    uint8_t dr = dataRate;
    writeToRegister(BW_DATARATE_REGISTER, 1, &dr); 
}

void printDataRate()
{
    uint8_t registerContent = 0;
    getDataRate(&registerContent);
    if (registerContent == DATA_RATE_25_HZ) {
        ESP_LOGI(TAG, "Data Rate: 25hz");
        return;
    }
    if (registerContent == DATA_RATE_50_HZ) {
        ESP_LOGI(TAG, "Data Rate: 50hz");
        return;
    }
    if (registerContent == DATA_RATE_100_HZ) {
        ESP_LOGI(TAG, "Data Rate: 100hz");
        return;
    }
    if (registerContent == DATA_RATE_200_HZ) {
        ESP_LOGI(TAG, "Data Rate: 200hz");
        return;
    }
    if (registerContent == DATA_RATE_400_HZ) {
        ESP_LOGI(TAG, "Data Rate: 400hz");
        return;
    }
    if (registerContent == DATA_RATE_800_HZ) {
        ESP_LOGI(TAG, "Data Rate: 800hz");
        return;
    }
    if (registerContent == DATA_RATE_1600_HZ) {
        ESP_LOGI(TAG, "Data Rate: 1600hz");
        return;
    }
    if (registerContent == DATA_RATE_3200_HZ) {
        ESP_LOGI(TAG, "Data Rate: 3200hz");
        return;
    }
}