#include <stdio.h>
#include <stdbool.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"
#include "batteryState.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"

#define BATT_PIN 35
#define BATT_EXTERN_PWER_SOURCE 39

bool initialized = false;
static esp_adc_cal_characteristics_t adc1_chars;

uint8_t convertVoltageToBatteryLevel(uint32_t voltage)
{
    if (voltage < 1600)
        return 255;
    if (voltage > 2080)
        return 100;
    if (voltage > 2055)
        return 90;
    if (voltage > 2040)
        return 85;
    if (voltage > 2010)
        return 80;
    if (voltage > 1990)
        return 75;
    if (voltage > 1975)
        return 70;
    if (voltage > 1955)
        return 65;
    if (voltage > 1935)
        return 60;
    if (voltage > 1920)
        return 50; 
    if (voltage > 1900)
        return 40;
    if (voltage > 1885)
        return 30;
    if (voltage > 1865)
        return 20;
    if (voltage > 1845)
        return 10;
    return 5; // 5% since the device will power off anyway at 0%
}

int bt_battery_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (initialized == false)
    {
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
        ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
        ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11));
        gpio_set_direction(BATT_EXTERN_PWER_SOURCE, GPIO_MODE_INPUT);
        initialized = true;
    }

    int adcRaw = adc1_get_raw(ADC1_CHANNEL_7);
    printf("battery %d \n", adcRaw);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adcRaw, &adc1_chars);
    bool charging = gpio_get_level(BATT_EXTERN_PWER_SOURCE);
    struct BatteryState batteryState = {
        .charging = charging,
        .level = convertVoltageToBatteryLevel(voltage)
    };
    os_mbuf_append(ctxt->om, &batteryState, sizeof(struct BatteryState));
    ESP_LOGI("BT Characteristics", "Read battery state");

    return 0;
}