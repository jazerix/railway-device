#include <stdio.h>
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_hs.h"
#include "../../state.h"
#include "../../accelerometer/offset.h"

int bt_calibration_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    {
        os_mbuf_append(ctxt->om, &offsets, sizeof(struct OffsetData));
        ESP_LOGI("BT Characteristics", "Read calibration offsets");
    }
    else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {
        ESP_LOGI("BT Characteristics", "Calibrating sensor");
        calibrateSensor();
    }
    return 0;
}