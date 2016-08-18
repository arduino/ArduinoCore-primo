/*
  Copyright (c) 2016 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

class BLEAdvertisement {
public:
	BLEAdvertisement(void);

	bool setFlags(uint32_t bleAdvFlags);
    bool setLocalName(const char* localName);
    bool setAdvertisedServiceData(const char* serviceDataUuid, uint8_t* serviceData, uint8_t serviceDataLength);
	bool setAdvertisedServiceUUID(const char* serviceUuid);
	bool setTxPower(int8_t txPower);
	void enableScanResponse(bool enable);
	
private:
	bool scanResponseEnabled = true;
};