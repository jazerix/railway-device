#include <stdio.h>
#include <stdbool.h>
#include "status.h"
#include "storage.h"
#include "accelerometer/accelerometer.h"
#include "accelerometer/offset.h"
#include "accelerometer/calibration.h"
#include "esp_log.h"
#include "state.h"
#include "esp_timer.h"
#include "writeBuffer.h"
#include "sdcard.h"

#define TAG "State"

static bool connected = false;

extern uint8_t state = STATE_NOT_CALIBRATED;
extern uint32_t recordingId = 0;
extern int64_t startTime = 0;

extern uint32_t samplesPrimary = 0;
extern uint32_t samplesSecondary = 0;

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
    bool calibrationSuccess = startCalibration();
    setStatus(LED_IDLE);
    if (!calibrationSuccess)
    {
        ESP_LOGW(TAG, "Calibration failed, please reorient device");
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
    samplesPrimary = 0;
    samplesSecondary = 0;
    recordingId = getRecordCouter();
    startTime = esp_timer_get_time();
    initWriteBuffer();
    createFile(recordingId);
    setStatus(LED_RECORDING);
    startMeasurement();
    state = STATE_RECORDING;
}

void stopRecording()
{
    if (state != 2)
        return;
    stopMeasurement();
    deinitWriteBuffer();
    closeCurrentFile();
    setStatus(LED_IDLE);
    recordingId = 0;
    state = STATE_READY;
}

void exitWithError()
{
    stopRecording();
    setStatus(LED_ERROR);
}