#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"

int bt_samples_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint32_t total = samplesPrimary + samplesSecondary;
    int status = os_mbuf_append(ctxt->om, &total, sizeof(total));
    ESP_LOGI("BT Characteristics", "Requested samples (%d)", total);

    return 0;
}