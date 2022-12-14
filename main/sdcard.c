#include <stdio.h>
#include <sys/unistd.h>
#include <esp_log.h>
#include <sys/stat.h>
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include <string.h>
#include "state.h"
#include "accelerometer/measurement.h"

#define PIN_NUM_MISO 26
#define PIN_NUM_MOSI 27
#define PIN_NUM_CLK 25
#define PIN_NUM_CS 33
#define MOUNT_POINT "/sdcard"

#define TAG "SDCard"

FILE *f;

void initSd()
{
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *card;

    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Intializing SD Card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE("SDCARD", "Failed to mount filesystem. "
                               "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE("SDCARD", "Failed to initialize the card (%s). "
                               "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        exitWithError();
        return;
    }
    ESP_LOGI(TAG, "Filesystem Mounted");
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "Initialized successfully");
}

void createFile(int recordingId)
{
    char fileName[100];
    snprintf(fileName, 100, MOUNT_POINT"/%d.bin", recordingId);
    f = fopen(&fileName, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s for writing", fileName);
        return;
    }
    ESP_LOGI(TAG, "File %s opened for writing.", fileName);
}

void closeCurrentFile()
{
    fclose(f);
    ESP_LOGI(TAG, "File closed.");
}

void writeSensorData(struct AccData *sensorData)
{
    fwrite(sensorData, sizeof(struct AccData),1000, f);
}