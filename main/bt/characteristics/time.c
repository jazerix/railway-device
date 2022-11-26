#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"
#include "esp_timer.h"

int bt_time_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int64_t currentTime = esp_timer_get_time();
    int smallerCurrentTime = (unsigned int)((currentTime - startTime)  / 1000);
    os_mbuf_append(ctxt->om, &smallerCurrentTime, sizeof(smallerCurrentTime));
    ESP_LOGI("BT Characteristics", "Read time");

    return 0;
}