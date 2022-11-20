#include <stdio.h>
#include <driver/i2c.h>
#include "esp_types.h"
#include "esp_log.h"
#include "accelerometer.h"

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

#define PWR_CTRL_REGISTER 0x2d
#define DATA_FORMAT_REGISTER 0x31
#define DATA_REGISTER 0x32
#define QUEUE_CTRL_REGISTER 0x38
#define QUEUE_REGISTER 0x39

#define SDA_GPIO 18
#define SCL_GPIO 19
#define ADDR 0x53
#define TAG "Accelerometer"

i2c_config_t i2c_config;

void initAccelerometer()
{
    ESP_LOGI(TAG, "Initializing");
    configureAccelerometer();
    initializeDataFormat();

    uint8_t dataFormat = 0;
    getDataFormat(&dataFormat);
    ESP_LOGI(TAG, "Current data format: %x", dataFormat); 
    /*uint8_t deviceId[1];

    getDeviceId(&deviceId);
    ESP_LOGI(TAG, "Reading device id: %x", deviceId[0]);

    uint8_t powerConfig = 0;
    getPowerControl(&powerConfig);
    ESP_LOGI(TAG, "Power: %x", powerConfig);

    uint8_t queueStatus = 0;
    getQueueStatus(&queueStatus);
    ESP_LOGI(TAG, "Current queue status: %x", queueStatus);

    startMeasureMode();
    uint8_t queueConfig = 0b01000000;
    writeToRegister(QUEUE_CTRL_REGISTER, 1, &queueConfig);

    getPowerControl(&powerConfig);
    ESP_LOGI(TAG, "Power: %x", powerConfig);

    getQueueStatus(&queueStatus);
    ESP_LOGI(TAG, "Current queue status: %x", queueStatus);*/

    startMeasureMode();

    //uint8_t data[6];
    //while(1)
       getCurrentData();

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

void getPowerControl(uint8_t *powerControl)
{
    readFromRegister(PWR_CTRL_REGISTER, 1, powerControl);
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

void startMeasureMode()
{
    uint8_t data = 0x08;
    writeToRegister(PWR_CTRL_REGISTER, 1, &data);
}

struct AccData {
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

struct AccData* getCurrentData()
{
    uint8_t data[6];
    readFromRegister(DATA_REGISTER, 6, &data);

    int16_t dataX = (data[1] << 8 | data[0]);
    int16_t dataY = (data[3] << 8 | data[2]);
    int16_t dataZ = (data[5] << 8 | data[4]);

    double lsb = 0.00390625 ; // -+2 = 4 / (2^10) = 0.00390625
    double dataXLSBPerG = dataX * lsb; 
    double dataYLSBPerG = dataY * lsb;
    double dataZLSBPerG = dataZ * lsb;
    

    ESP_LOGI(TAG, "X%lf", dataZLSBPerG);

    return (struct AccData*) data;
}

void readFromRegister(uint8_t register_location, int register_read_length, uint8_t *data)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);

    i2c_master_write_byte(cmd_handle, (ADDR << 1) | WRITE_BIT, ACK_CHECK_EN); // select address
    i2c_master_write_byte(cmd_handle, register_location, ACK_CHECK_EN);       // select register
    i2c_master_start(cmd_handle);                                             // send queued items

    i2c_master_write_byte(cmd_handle, ADDR << 1 | READ_BIT, ACK_CHECK_EN);

    if (register_read_length > 1)
        i2c_master_read(cmd_handle, data, register_read_length - 1, ACK_VAL);

    i2c_master_read_byte(cmd_handle, data + register_read_length - 1, NACK_VAL);

    i2c_master_stop(cmd_handle);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret == ESP_OK)
        return;
    if (ret == ESP_ERR_TIMEOUT)
    {
        ESP_LOGW(TAG, "Bus is busy");
    }
    else
    {
        ESP_LOGW(TAG, "Read Failed");
    }
}

void writeToRegister(uint8_t register_location, int write_length, uint8_t *value)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd_handle, register_location, ACK_CHECK_EN);
    for (int i = 0; i < write_length; i++)
        i2c_master_write_byte(cmd_handle, value[i], ACK_CHECK_EN);
    i2c_master_stop(cmd_handle);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret == ESP_OK)
        return;
    if (ret == ESP_ERR_TIMEOUT)
    {
        ESP_LOGW(TAG, "Bus is busy");
    }
    else
    {
        ESP_LOGW(TAG, "Write Failed");
    }
}