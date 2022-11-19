#include <stdio.h>
#include "neopixel.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "state.h"

#define LED_PIN 15

struct colors
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;
    uint8_t other;
};

const struct colors OFF = {
    .green = 0x00,
    .red = 0x00,
    .blue = 0x00,
    .other = 0x00};

const struct colors BLUE = {
    .green = 0x00,
    .red = 0x00,
    .blue = 0xff,
    .other = 0xff};

const struct colors RED = {
    .green = 0x00,
    .red = 0xff,
    .blue = 0x00,
    .other = 0xff};

const struct colors GREEN = {
    .green = 0xff,
    .red = 0x00,
    .blue = 0x00,
    .other = 0xff};

const struct colors PURPLE = {
    .green = 0x00,
    .red = 0xff,
    .blue = 0xff,
    .other = 0xff};

const struct colors ERROR = {
    .green = 0xa5,
    .red = 0xff,
    .blue = 0x00,
    .other = 0xff};

const int LED_AWAITING_CONNECTION = 1;
const int LED_IDLE = 2;
const int LED_RECORDING = 3;
const int LED_ERROR = 4;

TaskHandle_t xBlueLedHandler = NULL;
TaskHandle_t xErrorLedHandler = NULL;

void blueBlinking(void *params)
{
    bool isOn = true;
    struct colors *colorToUse = recording ? &PURPLE : &BLUE;
    while (true)
    {
        espShow(LED_PIN, isOn ? colorToUse : &OFF, 4, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        isOn = !isOn;
    }
    vTaskDelete(NULL);
    xBlueLedHandler = NULL;
}

void errorBlinking(void *params)
{
    bool isOn = true;
    while (true)
    {
        espShow(LED_PIN, isOn ? &ERROR : &OFF, 4, 1);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        isOn = !isOn;
    }
    vTaskDelete(NULL);
    xBlueLedHandler = NULL;
}


void setStatus(int state)
{
    if (xBlueLedHandler != NULL)
    {
        vTaskDelete(xBlueLedHandler);
        xBlueLedHandler = NULL;
    }
    if (xErrorLedHandler != NULL)
    {
        vTaskDelete(xErrorLedHandler);
        xErrorLedHandler = NULL;
    }

    if (state == LED_AWAITING_CONNECTION)
    {
        xTaskCreate(blueBlinking, "waiting for connection", 2000, NULL, 2, &xBlueLedHandler);
        return;
    }

    if (state == LED_IDLE)
    {
        espShow(LED_PIN, &GREEN, 4, 1);
        return;
    }

    if (state == LED_RECORDING)
    {
        espShow(LED_PIN, &RED, 4, 1);
        return;
    }

    if (state == LED_ERROR) {
        xTaskCreate(errorBlinking, "error_occured", 2000, NULL, 2, &xErrorLedHandler);
        return;
    }
}