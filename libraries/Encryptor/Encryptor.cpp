/*
  Encryptor class for nRF52.
  Written by Chiara Ruggeri (chiara@arduino.org)
  
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

  Enjoy!  
*/


#include "Encryptor.h"


bool EncryptorClass::encrypt(const char key [], const char cleartext [], char ciphertext[], uint32_t buff_len){
	uint8_t i;
	uint32_t err_code;
	uint32_t kLen = strlen(key);
	if(kLen > 16)
		kLen = 16;

	nrf_ecb_hal_data_t ecb_struct;
	memset(&ecb_struct, 0, sizeof(ecb_struct));

	// initializing key
	for(i = 0; i < kLen; i++)
		ecb_struct.key[i] = key[i];
	
	// initializing nouncence
	memset(ecb_struct.cleartext, 0xF0, sizeof(ecb_struct.cleartext));
	err_code = sd_ecb_block_encrypt(&ecb_struct);
	
	ecb_struct.ciphertext[0] += (uint8_t)_tx_counter;
	ecb_struct.ciphertext[1] += (uint8_t)(_tx_counter>>8);
	ecb_struct.ciphertext[2] += (uint8_t)(_tx_counter>>16);
	ecb_struct.ciphertext[3] += (uint8_t)(_tx_counter>>24);
	_tx_counter++;
	
	// ecb_struct.ciphertext
	
	uint8_t j=0;
	// Encrypt 
	for(i = 0; i < buff_len-1; i++){
		ciphertext[i] = cleartext[i] ^ ecb_struct.ciphertext[j];
		ecb_struct.ciphertext[j]+=i;
		if(++j==16)
			j=0;
	}
	
	// Most probably the value will be treated as string. Add the terminator character
	ciphertext[i] = '\0';
	
	// check for error and return
	return err_code == 0 ? true : false;
}



bool EncryptorClass::decrypt(const char key [], const char ciphertext [], char cleartext [], uint32_t buff_len){
	uint8_t i;
	uint32_t err_code;
	uint32_t kLen = strlen(key);
	if(kLen > 16)
		kLen = 16;

	nrf_ecb_hal_data_t ecb_struct;
	memset(&ecb_struct, 0, sizeof(ecb_struct));
	
	// initializing key
	for(i = 0; i < kLen; i++)
		ecb_struct.key[i] = key[i];
	
	// initializing nouncence
	memset(ecb_struct.cleartext, 0xF0, sizeof(ecb_struct.cleartext));
	err_code=sd_ecb_block_encrypt(&ecb_struct);
	
	ecb_struct.ciphertext[0] += (uint8_t)_rx_counter;
	ecb_struct.ciphertext[1] += (uint8_t)(_rx_counter>>8);
	ecb_struct.ciphertext[2] += (uint8_t)(_rx_counter>>16);
	ecb_struct.ciphertext[3] += (uint8_t)(_rx_counter>>24);
	_rx_counter++;
	
	uint8_t j=0;
	// Decrypt 
	for(i = 0; i < buff_len-1; i++){
		cleartext[i] = ciphertext[i] ^ ecb_struct.ciphertext[j];
		ecb_struct.ciphertext[j]+=i;
		if(++j==16)
			j=0;
	}
	
	// Most probably the value will be treated as string. Add the terminator character
	cleartext[i] = '\0';
	
	// check for error and return
	return err_code == 0 ? true : false;
}

EncryptorClass Encryptor;