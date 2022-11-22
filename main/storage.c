#include <stdio.h>
#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG "Storage"

uint32_t getRecordCouter()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_LOGW(TAG, "Flash cleared");
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
    nvs_handle_t counter_handle;
    err = nvs_open("storage", NVS_READWRITE, &counter_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 0;
    }

    uint32_t record_count = 0;
    nvs_get_u32(counter_handle, "rec_id", &record_count);
    record_count++;
    err = nvs_set_u32(counter_handle, "rec_id", record_count);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to update recording counter (%s)", esp_err_to_name(err));

    err = nvs_commit(counter_handle);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to commit changes to flash");
    nvs_close(counter_handle);

    return record_count;
}