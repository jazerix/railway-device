#ifndef _GLOBALSTATE_
#define _GLOBALSTATE_
#define STATE_NOT_CALIBRATED 0
#define STATE_READY 1
#define STATE_RECORDING 2


extern uint8_t state;
extern uint32_t recordingId;
extern uint32_t samplesPrimary;
extern uint32_t samplesSecondary;
extern struct OffsetData offsets;
extern int64_t startTime;

void setConnected(bool isConnected);
void startRecording();
void stopRecording();
void exitWithError();
void calibrateSensor();
#endif