#ifndef _GLOBALSTATE_
#define _GLOBALSTATE_

extern bool recording;
extern uint32_t recordingId;
extern struct OffsetData offsets;

void setConnected(bool isConnected);
void startRecording();
void stopRecording();
void exitWithError();
#endif