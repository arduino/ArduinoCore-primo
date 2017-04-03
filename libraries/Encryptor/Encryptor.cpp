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

EncryptorClass::EncryptorClass(void){
    memset(_key, 0,    ENC_KEY_SIZE);
    memset(_iv,  0xF0, ENC_KEY_SIZE);
    _counter = 0;
    for(int i = 0; i < MIC_SIZE; i++) _micValue[i] = (char)(i * 39);
}

bool EncryptorClass::encrypt(const char cleartext [], char ciphertext[], uint32_t buff_len){
	uint32_t err_code;

	nrf_ecb_hal_data_t ecb_struct;
	memset(&ecb_struct, 0, sizeof(ecb_struct));

    // initializing key
    memcpy(ecb_struct.key, _key, ENC_KEY_SIZE);
        
    for(int i = 0; i < buff_len; i += ENC_KEY_SIZE){
        // initializing nounce
        memcpy(ecb_struct.cleartext, _iv, ENC_KEY_SIZE);
        ecb_struct.cleartext[0] ^= (uint8_t)_counter;
        ecb_struct.cleartext[1] ^= (uint8_t)(_counter >> 8);
        ecb_struct.cleartext[2] ^= (uint8_t)(_counter >> 16);
        ecb_struct.cleartext[3] ^= (uint8_t)(_counter >> 24);
        
        // Auto increment the counter between each block
        _counter++;
        
        err_code = sd_ecb_block_encrypt(&ecb_struct);
        if(err_code != 0) return false;
                
        // Encrypt 
        int blockDataLength = ((buff_len - i) >= 16) ? 16 : (buff_len - i);
        for(int j = 0; j < blockDataLength; j++){
            ciphertext[i + j] = cleartext[i + j] ^ ecb_struct.ciphertext[j];
        }
    }
	
	return true;
}



bool EncryptorClass::decrypt(const char ciphertext [], char cleartext [], uint32_t buff_len){
	uint32_t err_code;

	nrf_ecb_hal_data_t ecb_struct;
	memset(&ecb_struct, 0, sizeof(ecb_struct));
	
	// initializing key
    memcpy(ecb_struct.key, _key, ENC_KEY_SIZE);
	
    for(int i = 0; i < buff_len; i += ENC_KEY_SIZE){
            
        // initializing nounce
        memcpy(ecb_struct.cleartext, _iv, ENC_KEY_SIZE);
        ecb_struct.cleartext[0] ^= (uint8_t)_counter;
        ecb_struct.cleartext[1] ^= (uint8_t)(_counter >> 8);
        ecb_struct.cleartext[2] ^= (uint8_t)(_counter >> 16);
        ecb_struct.cleartext[3] ^= (uint8_t)(_counter >> 24);
        
        // Auto increment the counter between each block
        _counter++;
        
        err_code = sd_ecb_block_encrypt(&ecb_struct);
        if(err_code != 0) return false;
        
        // Decrypt 
        int blockDataLength = ((buff_len - i) >= 16) ? 16 : (buff_len - i);
        for(int j = 0; j < blockDataLength; j++){
            cleartext[i + j] = ciphertext[i + j] ^ ecb_struct.ciphertext[j];
        }
	}

	return true;
}

void EncryptorClass::setKey(const char *key){
    memcpy(_key, key, ENC_KEY_SIZE);
}

void EncryptorClass::setInitVector(const char *iv){
    memcpy(_iv, iv, ENC_KEY_SIZE);
}

void EncryptorClass::setCounter(uint32_t counter){
    _counter = counter;
}

uint32_t EncryptorClass::getCounter(void){
    return _counter;
}

void enableMic(bool enabled){
    _micEnabled = enabled;
}

EncryptorClass Encryptor;