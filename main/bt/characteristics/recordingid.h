#ifndef _BTCHARACTERISTICRECID_
#define _BTCHARACTERISTICRECID_
#include <stdint.h>

int bt_recording_id_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

#endif