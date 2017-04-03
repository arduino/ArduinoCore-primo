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

#ifndef Encryptor_h
#define Encryptor_h

#include "Arduino.h"
#include "nrf_ecb.h"

#define ENC_KEY_SIZE    16
#define MIC_SIZE        2

class EncryptorClass{
	
	public:
		
        EncryptorClass(void);
        
		/**
		* @brief
		* Name:
		*			encrypt
		* Description:
		*			Function for encrypting data
		* Arguments:
		*			-key: encryption key [16 byte maximum]
		*			-cleartext: data to be encrypted
		*			-ciphertext: result of the encryption process
		* Returned value:
		*			-ture  - encryption succeeded
		*			-false - encryption failed
		*/
		bool encrypt(const char cleartext [], char ciphertext [], uint32_t buff_len);
		
		/**
		* @brief
		* Name:
		*			decrypt
		* Description:
		*			Function for decrypting data
		* Arguments:
		*			-key: encryption key [16 byte maximum]
		*			-ciphertext: data to be encrypted
		*			-cleartext: result of the decryption process
		* Returned value:
		*			-ture  - decryption succeeded
		*			-false - decryption failed		
		*/
		bool decrypt(const char ciphertext [], char cleartext [], uint32_t buff_len);
	
        void setKey(const char *key);
        
        void setInitVector(const char *iv);
        
        void setCounter(uint32_t counter);
        
        uint32_t getCounter(void);
        
	private:        
        char     _key[ENC_KEY_SIZE];
        char     _iv[ENC_KEY_SIZE];
        uint32_t _counter;
        bool     _micEnabled;
        char     _micValue[MIC_SIZE];
};

extern EncryptorClass Encryptor;

#endif Encryptor_h