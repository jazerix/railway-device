#ifndef _BTCHARACTERISTICSTATE_
#define _BTCHARACTERISTICSTATE_

int bt_state_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

#endif