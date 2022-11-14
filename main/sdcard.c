#include <stdio.h>
#include <sys/unistd.h>
#include <esp_log.h>
#include <sys/stat.h>
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include <string.h>


#define PIN_NUM_MISO  18
#define PIN_NUM_MOSI  19
#define PIN_NUM_CLK   21
#define PIN_NUM_CS    23
#define MOUNT_POINT "/sdcard"

void initSd()
{
    esp_err_t ret;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;

    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI("SDCARD", "Intializing SD Card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 4000
    };

     ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);

     if (ret != ESP_OK)
     {
        ESP_LOGE("SDCARD", "Failed to initialize bus");
        return;
     }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI("SDCARD", "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("SDCARD", "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE("SDCARD", "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI("SDCARD", "Filesystem Mounted");
    sdmmc_card_print_info(stdout, card);

    const char *file_hello = MOUNT_POINT"/hello.txt";

    ESP_LOGI("SDCARD", "Reading file %s", file_hello);
    FILE *fr = fopen(file_hello, "r");
    if (fr == NULL) {
        ESP_LOGE("SDCARD", "Failed to open file for reading");
        goto write;
    }
    char line[64];
    fgets(line, sizeof(line), fr);
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI("SDCARD", "Read from file: '%s'", line);
    fclose(fr);


    write:

    /*/ESP_LOGI("SDCARD", "Opening file for writing: %s", file_hello);
    FILE *f = fopen(file_hello, "w");
    if (f == NULL) {
        ESP_LOGE("SDCARD", "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello %s!\n", card->cid.name);
    fclose(f);
    ESP_LOGI("SDCARD", "File written");*/

    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI("SDCARD", "Card unmounted");

    //deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);
}