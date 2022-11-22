#ifndef _ACCELEROMETER_COMMUNICATION_
#define _ACCELEROMETER_COMMUNICATION_

void readFromRegister(uint8_t register_location, int register_read_length, uint8_t *data);
void writeToRegister(uint8_t register_location, int write_length, uint8_t *value);

#endif