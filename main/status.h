#ifndef _STATUS_
#define _STATUS_

const int LED_AWAITING_CONNECTION;
const int LED_IDLE;
const int LED_RECORDING;
const int LED_ERROR;

void setStatus(int state);
#endif