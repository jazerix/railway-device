#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"

int bt_recording_id_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int recId = recordingId;
    int status = os_mbuf_append(ctxt->om, &recId, sizeof(recId));
    ESP_LOGI("BT Characteristics", "Requested recording id (%d)", recId);
    
    return 0;
}