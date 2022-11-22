#include <stdio.h>
/*#include "status.h"
#include "bt/connection.h"
#include "sdcard.h"
#include "accelerometer/accelerometer.c"
*/
#include "storage.h"
#include <inttypes.h>

void app_main(void)
{
    /*initAccelerometer();
    initializeBluetooth();
    setStatus(LED_AWAITING_CONNECTION);
    initSd();*/

    printf("test: %" PRIu32 "\n", getRecordCouter());
    // ESP_LOGI("TIMER", "Current time: %" PRId64, esp_timer_get_time());
}
