#include <stdio.h>
#include <driver/i2c.h>
#include "communication.h"
#include "esp_log.h"

#define ADDR 0x53
#define TAG "Accelerometer"

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */


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