#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "status.h"
#include "bt/connection.h"

void app_main(void)
{
    initializeBluetooth();
    setStatus(LED_AWAITING_CONNECTION);
}