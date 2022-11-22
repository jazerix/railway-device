#include <stdio.h>
#define INTR_PIN 34
#define INTR_ENABLE 0x2e
#define INTR_SOURCE 0x30

void setupInterrupts()
{

    enableWatermarkInterrupt();
}

void enableWatermarkInterrupt()
{
    uint8_t dataToWrite = 0b00000010;
    writeToRegister(INTR_ENABLE, 1, &dataToWrite);
}

void getInterruptStatus(uint8_t *status)
{
    readFromRegister(INTR_SOURCE, 1, status);
}