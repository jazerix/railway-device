#ifndef _GLOBALSTATE_
#define _GLOBALSTATE_

extern bool recording;
extern int passed_time;

void setConnected(bool isConnected);
void startRecording();
void stopRecording();
#endif