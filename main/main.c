#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "status.h"
#include "bt/connection.h"
#include "sdcard.h"
#include "accelerometer/accelerometer.c"

void app_main(void)
{
    initAccelerometer();
    //initializeBluetooth();
    //setStatus(LED_AWAITING_CONNECTION);
    //initSd();
}

// 1400 / 67 = 20 timer
// 3000 / 67 = 45