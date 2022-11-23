#ifndef _BTCHARACTERISTICTIME_
#define _BTCHARACTERISTICTIME_
#include <stdint.h>

int bt_time_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

#endif