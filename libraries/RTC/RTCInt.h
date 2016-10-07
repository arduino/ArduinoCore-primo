/*
  RTC library for Arduino Primo.
  Copyright (c) 2016 Arduino. All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef RTCInt_H
#define RTCInt_H

#define TIME_H24 1
#define TIME_H12 0

#define OFF 0
#define SEC 1
#define MMSS 2
#define HHMMSS 3
#define DDHHMMSS 4
#define MMDDHHMMSS 5
#define YYMMDDHHMMSS 6

#define ALARM_POLLED 0
#define ALARM_INTERRUPT 1

#define ANTI_MERIDIAN 0
#define POST_MERIDIAN 1

//Parameter for oscillatorType - M0 only
// #define HIGH_PRECISION 0
// #define LOW_POWER 1

#include "Arduino.h"
#include "nrf_rtc.h"
#include "nrf_clock.h"

typedef struct {
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned char Tmode;
}TIME;

typedef struct {
	unsigned int day;
	unsigned int month;
	unsigned int year;
}DATE;

typedef struct {
	bool active=0;
	unsigned int type;
	unsigned int addDay=0;
	bool addMonth=0;
	uint32_t sec_to_next_int=0;
	uint32_t seconds;
	bool set=0;
}ALARM;

static voidFuncPtr _callback = NULL;

class RTCInt {
public:

  RTCInt() {};
  
  TIME time;
  DATE date;
  
  //internal structure to count the time
  TIME timeInt;
  DATE dateInt;
  ALARM alarm;
  
//  void begin(bool timeMode, int oscillatorType);
  void begin(bool timeMode);
  
  /* Get Time Functions */

  unsigned int getHour(void);
  unsigned int getMinute(void);
  unsigned int getSecond(void);
  void getTime(void); 
  unsigned char getMeridian(void); 
 
 /* Get Date Functions */ 
  unsigned int getDay(void);
  unsigned int getMonth(void);
  unsigned int getYear(void);
  void getDate(void);

  /* Set Time Functions */

  void setHour(unsigned int hour, unsigned char meridian);
  void setMinute(unsigned int minute);
  void setSecond(unsigned int second);
  void setTime(unsigned int hour, unsigned char meridian, unsigned int minute, unsigned int second);
  void setTime(void); 

 /* Set Date Functions */ 
  void setDay(unsigned int day);
  void setMonth(unsigned int month);
  void setYear(unsigned int year);
  void setDate(unsigned int day, unsigned int month, unsigned int year);
  void setDate(void);  
  
 /* Alarm Functions */
	void enableAlarm(unsigned int mode, unsigned int type, voidFuncPtr callback);
	void setAlarm(void);
	bool alarmMatch(void);
	void alarmClearFlag(void);
	
  unsigned int rdn(int y, int m, int d);
  
  
private:
  unsigned int Alarm_Mode=0;
  bool RTCSync(void);  
  void RTCdisable(void);
  void RTCenable(void);
  void RTCreset(void);
  void RTCresetRemove(void);
  
};


#endif