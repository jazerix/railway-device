#include <stdio.h>
#include "stdbool.h"
#include "esp_random.h"
#include "status.h"

static char* recordingId;
static int batteryLevel = 100;
static int samples = 0;
static bool connected = false;

extern bool recording = false;
extern int passed_time = 0;

void setConnected(bool isConnected)
{
    connected = isConnected;
}

void startRecording()
{
    if (connected == false)
        return;
    setStatus(LED_RECORDING);
    recording = true;
    //recording = "test";
}

void stopRecording()
{
    setStatus(LED_IDLE);
    recording = false;
}