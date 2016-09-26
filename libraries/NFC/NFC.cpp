/*
  NFC class for nRF52.
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


#include "NFC.h"


void nfc_callback(void *context, NfcEvent event, const char *data, size_t dataLength)
{
	NFC.onService();
}


uint8_t ndef_msg_buf[256];

void NFCClass::begin(){
	SDManager.begin();
	nfcSetup(nfc_callback, NULL);
}

void NFCClass::setTXTmessage(const uint8_t TXTMessage[], const uint8_t language[]){

	uint32_t len = sizeof(ndef_msg_buf);

	char message[256];
	strcpy(message, (char *)TXTMessage);
	uint8_t sizeM=strlen(message);

	char lang[256];
	strcpy(lang, (char *)language);
	uint8_t sizeL=strlen(lang);

	NFC_NDEF_MSG_DEF(welcome_msg, 1);	
    NFC_NDEF_TEXT_RECORD_DESC_DEF(en_text_rec,
                                  UTF_8,
                                  (uint8_t *)lang,
                                  sizeL,
                                  (uint8_t *)message,
                                  sizeM);
	nfc_ndef_msg_record_add( &NFC_NDEF_MSG(welcome_msg),
                                        &NFC_NDEF_TEXT_RECORD_DESC(en_text_rec));
		
	nfc_ndef_msg_encode(&NFC_NDEF_MSG(welcome_msg),
                                            ndef_msg_buf,
                                            &len);
	 
	nfcSetPayload((char *) ndef_msg_buf, len);
	nfcStartEmulation();
}

void NFCClass::setURImessage( const char URL[], nfc_uri_id_t type){
	char url[256];
	strcpy(url, URL);
	uint8_t size=strlen(url);
	uint32_t len = sizeof(ndef_msg_buf);
	nfc_uri_msg_encode( type,
                         (uint8_t *) url,
                         size,
                         ndef_msg_buf,
                         &len);
								   
	nfcSetPayload( (char*)ndef_msg_buf, len);
	nfcStartEmulation();
	
}

void NFCClass::setAPPmessage(const uint8_t android_app[], const uint8_t windows_app[]){
	
	char Aapp[256];
	strcpy(Aapp, (char *)android_app);
	uint8_t sizeA=strlen(Aapp);

	char Wapp[256];
	strcpy(Wapp, (char *)windows_app);
	uint8_t sizeW=strlen(Wapp);

	uint32_t len = sizeof(ndef_msg_buf);
	
	nfc_launchapp_msg_encode((uint8_t *)Aapp,
                                    sizeA,
                                    (uint8_t *)Wapp,
                                    sizeW,
                                    ndef_msg_buf,
                                    &len);
  
	nfcSetPayload((char *) ndef_msg_buf, len);
	nfcStartEmulation();
}

void NFCClass::registerCallback(void(*function)(void)){
	Callback=function;
}

void NFCClass::onService(){
	if(Callback)
		Callback();
 }



NFCClass NFC;
