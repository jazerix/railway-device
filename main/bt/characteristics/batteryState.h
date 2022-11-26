#ifndef _BATTERY_STATE_CHARACTERISTIC_
#define _BATTERY_STATE_CHARACTERISTIC_
#include <stdbool.h>
struct BatteryState {
    bool charging;
    uint8_t level;
};
#endif