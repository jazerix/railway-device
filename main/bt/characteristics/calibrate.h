#ifndef _CALIBRATECHARACTERISTIC_
#define _CALIBRATECHARACTERISTIC_
#include <stdint.h>

int bt_calibration_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

#endif