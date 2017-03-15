#include "CIR.h"

CIR::CIR(void) {

}

void CIR::enableReceiver(void) {
	  delay(100);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA0);
	  Wire.endTransmission();
}

void CIR::disableReceiver(void) {
	  delay(15);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA1);
	  Wire.endTransmission();
}

bool CIR::getReceiverStatus(void) {
	 delay(150);
	 Wire.begin();
	 Wire.beginTransmission(0x48);
	 Wire.write(0xA2);
	 Wire.endTransmission();
	 delay(15);
	 Wire.requestFrom(0x48,4, true);
	 recBuffer[0] = Wire.read();

	 int i;

	 for (i=1;i<4;i++) {
		 recBuffer[i] = Wire.read();
	 }

	 if (recBuffer[0] == 0)
		 return false;
	 else
		 return true;
}

uint32_t CIR::getDecode(void) {
	uint32_t code=0;
	code  =  (recBuffer[0] << 24);
	code |=  (recBuffer[1] << 16);
	code |=  (recBuffer[2] <<  8);
	code |=  recBuffer[3];

	return code;
}

void CIR::enableTransmitter(void) {
	  delay(100);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA3);
	  Wire.endTransmission();
}

void CIR::disableTransmitter(void) {
	  delay(15);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA4);
	  Wire.endTransmission();

}

void CIR::sendEncode(uint32_t data) {

	  uint8_t transdata[4];
	  transdata[0] = (data >> 24);
	  transdata[1] = (data >> 16);
	  transdata[2] = (data >> 8);
	  transdata[3] = data;

	  delay(200);
	  Wire.begin();
	  Wire.beginTransmission(0x48);
	  Wire.write(0xA5);
	  Wire.write(transdata[0]);
	  Wire.write(transdata[1]);
	  Wire.write(transdata[2]);
	  Wire.write(transdata[3]);
	  Wire.endTransmission();
}
