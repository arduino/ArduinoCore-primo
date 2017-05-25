// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_BOND_STORE_H_
#define _BLE_BOND_STORE_H_

class BLEBondStore
{
  public:
    BLEBondStore(int offset = NRF_UICR->NRFFW[0]);

    bool hasData();
    void clearData();
    void putData(const unsigned char* data, unsigned int offset, unsigned int length);
    void getData(unsigned char* data, unsigned int offset, unsigned int length);
    void saveTempData(const unsigned char* data, unsigned int offset, unsigned int length);
    const unsigned char* getTempData();
    unsigned int getTempOffset();
	unsigned int getTempLength();

  private:
#if defined(__AVR__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    int             _offset;
#elif defined(NRF51)|| defined(NRF52) || defined(__RFduino__)
    uint32_t*       _flashPageStartAddress;
#endif
    const unsigned char*      _tempData;
    unsigned int              _tempOffset;
    unsigned int              _tempLength;
};

#endif
