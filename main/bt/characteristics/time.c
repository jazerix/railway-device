#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"

int bt_time_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /*if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    {*/
    os_mbuf_append(ctxt->om, &passed_time, sizeof(passed_time));
    ESP_LOGI("BT CHAR", "read time");
    /*}
    else
    {
        printf("%d", ctxt->op);
    }*/
    return 0;
}