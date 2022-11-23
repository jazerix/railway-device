#include <stdio.h>
#include "stdbool.h"
#include "status.h"
#include "storage.h"
#include "accelerometer/offset.h"
#include "esp_log.h"
#include "state.h"

#define TAG "State"

static int batteryLevel = 100;
static int samples = 0;
static bool connected = false;
static bool ready = false;

extern uint8_t state = 0;
extern uint32_t recordingId = 0;
extern struct OffsetData *offsets = NULL;

void setConnected(bool isConnected)
{
    connected = isConnected;
}

void startRecording()
{
    if (offsets == NULL)
    {
        ESP_LOGI(TAG, "Recoding can't be started as device isn't calibrated");
        return;
    }
    if (connected == false)
    {
        ESP_LOGI(TAG, "No connection has been made to the device, recording can't be started");
        return;
    }

    setStatus(LED_RECORDING);
    recordingId = getRecordCouter();
    state = STATE_RECORDING;
}

void stopRecording()
{
    setStatus(LED_IDLE);
    recordingId = 0;
    state = STATE_READY;
}

void exitWithError()
{
    stopRecording();
    setStatus(LED_ERROR);
}