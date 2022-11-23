#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"

int bt_state_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    {
        os_mbuf_append(ctxt->om, &state, sizeof(state));
        ESP_LOGI("BT CHAR", "read state");
    }
    else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {
        bool record = *ctxt->om->om_data;
         
        if (record)
        {
            startRecording();
        }
        else
        {
            stopRecording();
        }
        ESP_LOGI("BT CHAR", "write state");
    }
    return 0;
}