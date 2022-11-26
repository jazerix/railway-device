#include <stdio.h>
#include "communication.h"
#include "esp_log.h"
#include "measurement.h"
#include "esp_timer.h";
#include "../state.h"

#define TAG "Accelerometer"
#define PWR_CTRL_REGISTER 0x2d
#define DATA_REGISTER 0x32

void startMeasureMode()
{
    uint8_t data = 0x08;
    writeToRegister(PWR_CTRL_REGISTER, 1, &data);
}

void stopMeasureMode()
{
    uint8_t data = 0x04;
    writeToRegister(PWR_CTRL_REGISTER, 1, &data);
}

void getPowerControl(uint8_t *powerControl)
{
    readFromRegister(PWR_CTRL_REGISTER, 1, powerControl);
}


struct AccData getCurrentData()
{
    uint8_t data[6];
    
    readFromRegister(DATA_REGISTER, 6, &data);

    int16_t dataX = (data[1] << 8 | data[0]);
    int16_t dataY = (data[3] << 8 | data[2]);
    int16_t dataZ = (data[5] << 8 | data[4]);

    struct AccData accData;
    accData.x = dataX;
    accData.y = dataY;
    accData.z = dataZ;
    accData.time = (unsigned int)((esp_timer_get_time() - startTime) / 1000);

    return accData;
}

struct AccDataDecimal accConvertToDecimal(struct AccData accData)
{
    double lsb = 0.00390625; // -+2 = 4 / (2^10) = 0.00390625
    double dataXLSBPerG = accData.x * lsb;
    double dataYLSBPerG = accData.y * lsb;
    double dataZLSBPerG = accData.z * lsb;

    struct AccDataDecimal accDataDecimal;
    accDataDecimal.x = dataXLSBPerG;
    accDataDecimal.y = dataYLSBPerG;
    accDataDecimal.z = dataZLSBPerG;

    return accDataDecimal;
}

struct AccDataDecimal getCurrentDataAsDecimal()
{
    struct AccData data = getCurrentData();

    return accConvertToDecimal(data);
}

void printMeasurement(struct AccDataDecimal accDataDecimal)
{
    ESP_LOGI(TAG, "X%lf Y%lf Z%lf", accDataDecimal.x, accDataDecimal.y, accDataDecimal.z);
}
