#include <stdio.h>
#include "status.h"
#include "bt/connection.h"
#include "sdcard.h"
#include "accelerometer/accelerometer.c"

#include <inttypes.h>
#include "accelerometer/measurement.h"
void app_main(void)
{
    initAccelerometer();
    initializeBluetooth();
    setStatus(LED_AWAITING_CONNECTION);
    initSd();
}
