#include <stdio.h>
#include "stdbool.h"
#include "status.h"
#include "storage.h"
#include "accelerometer/accelerometer.h"
#include "accelerometer/offset.h"
#include "accelerometer/calibration.h"
#include "esp_log.h"
#include "state.h"
#include "esp_timer.h"
#include "writeBuffer.h"

#define TAG "State"

static int samples = 0;
static bool connected = false;

extern uint8_t state = STATE_NOT_CALIBRATED;
extern uint32_t recordingId = 0;
extern int64_t startTime = 0;
extern struct OffsetData offsets = {
    .x = 0,
    .y = 0,
    .z = 0
};

void setConnected(bool isConnected)
{
    connected = isConnected;
}

void calibrateSensor()
{
    setStatus(LED_CALIBRATING);
    startCalibration();
    setStatus(LED_IDLE);
    if (offsets.x == 0xff || offsets.y == 0xff || offsets.z == 0xff)
    {
        ESP_LOGE(TAG, "Calibration failed - please reseat device.");
        return;
    }
    state = STATE_READY;
}

void startRecording()
{
    if (state == STATE_NOT_CALIBRATED)
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
    startTime = esp_timer_get_time();
    initWriteQueue();
    startMeasurement();
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