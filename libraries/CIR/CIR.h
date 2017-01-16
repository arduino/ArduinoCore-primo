#ifndef __CIR_H__
#define __CIR_H__

#include <Wire.h>
#include <stdint.h>
#include "Arduino.h"

#define SEND_BUF_LENGTH 256
#define REC_BUF_LENGTH 74

class CIR {
public:
	CIR(void);
	void enableReciver(void);
	void disableReciver(void);
	bool getReciverStatus(void);
	uint32_t getDecode(void);

	void enableTransmitter(void);
	void disableTransmitter(void);
	void sendEncode(uint32_t data);

	uint16_t recData[34];
	uint16_t leader_mark, leader_space, mark_max, mark_min, space_max, space_min;

private:
	uint8_t sendBuffer[SEND_BUF_LENGTH];
	uint8_t recBuffer[REC_BUF_LENGTH];


};

#endif __CIR_H___
