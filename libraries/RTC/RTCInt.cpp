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
 
#include "RTCInt.h"

static bool time_Mode = false;

uint8_t count=0;
RTCInt *rtcPointer=NULL;
const uint8_t daysPerMonth[12][2]={{32, 32}, {29, 30}, {32, 32}, {31, 31}, {32, 32}, {31, 31}, {32, 32}, {32, 32}, {31, 31}, {32, 32}, {31, 31}, {32, 32}};
/*******************************************************************************************
*Description: Function responsible of RTC initialization
*Input Parameters: bool timeRep this parameter can be TIME_H24 (24 hour mode) or TIME_H12 (12 hour mode)
*Return Parameter: None
*******************************************************************************************/
void RTCInt::begin(bool timeRep){	
	//save this instance to be able to access to date and time
	//structures from ISR
	rtcPointer=this;
	//LFCLK needed to be started before using the RTC
	nrf_rtc_prescaler_set(NRF_RTC2, 4095);
	//enable interrupt
	NVIC_SetPriority(RTC2_IRQn, 2); //high priority
    NVIC_ClearPendingIRQ(RTC2_IRQn);
    NVIC_EnableIRQ(RTC2_IRQn);
	nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_TICK);
	nrf_rtc_int_enable(NRF_RTC2, NRF_RTC_INT_TICK_MASK);

	//start RTC
	nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_START);

	time_Mode=timeRep;
}

/*
 * Get Time Functions
 */

/**********************************************************************************************************************
*Description: Function for getting hour, according to time representation mode this value can 
			  be in the range 0-24 (in case of TIME_H24) or in the range 0-12 (in case of TIME_H12)	
*Input Parameters: None
*Return Parameter: unsigned int (hour)
***********************************************************************************************************************/
 unsigned int RTCInt::getHour()
{
	unsigned int h=0;
	
	h=timeInt.hour;
	if(time_Mode == TIME_H12)
	{
		if(h>12){
			h-=12;
			time.Tmode = 1;
		}
		else
			time.Tmode = 0;
	}
	
  return h;
}


/**********************************************************************************************************************
*Description: Function for getting minute, this value varies in the range 0 - 59 
*Input Parameters: None
*Return Parameter: unsigned int (minute)
***********************************************************************************************************************/
unsigned int RTCInt::getMinute()
{
  unsigned int m=0;
  
  m=timeInt.minute;
  return m; 
}


/**********************************************************************************************************************
*Description: Function for getting second, this value varies in the range 0 - 59 
*Input Parameters: None
*Return Parameter: unsigned int (second)
***********************************************************************************************************************/
 unsigned int RTCInt::getSecond()
{
  unsigned int s=0;
  
  s=timeInt.second;
  return s; 
}



void RTCInt::getTime(void)
{
	unsigned int hour=0, h=0;
	
	time.hour = getHour();
	time.minute = getMinute();
	time.second = getSecond();	
	
}

unsigned char RTCInt::getMeridian(void)
{
	unsigned int h=0;
	unsigned char m=0;
	
	if(time_Mode == TIME_H12)
	{	
		m=0;
		if(timeInt.hour>12)
			m = 1;
		
	}
	else m=0;
return m;	
}

/*
 * Get Date Functions
 */

 
/**********************************************************************************************************************
*Description: Function for getting day 	
*Input Parameters: None
*Return Parameter: unsigned int day in the range 1 - 31
***********************************************************************************************************************/ 
unsigned int RTCInt::getDay()
{
  unsigned int d=0;
  
  d=dateInt.day;
  return d; 
}

/**********************************************************************************************************************
*Description: Function for getting Month 	
*Input Parameters: None
*Return Parameter: unsigned int month in the range 1 - 12
***********************************************************************************************************************/
unsigned int RTCInt::getMonth()
{
  unsigned int month=0;
  
  month=dateInt.month;
  return month; 
}

/************************************************************************************************************************************
*Description: Function for getting year 	
*Input Parameters: None
*Return Parameter: unsigned int year in the range 0 - 63 (note: add offset to this value to obtain correct representation for year)
*************************************************************************************************************************************/
unsigned int RTCInt::getYear()
{
  unsigned int y=0;
  
  y=dateInt.year;
  return y; 
}

/**********************************************************************************************************************
*Description: Function for getting date (day, month and year). This function fills a structure called date
			  accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::getDate(void)
{
	date.day = getDay();
	date.month = getMonth();
	date.year = getYear();
}

/*
 * Set Time Functions
 */

 
/************************************************************************************************************************************
*Description: Function for setting hour 	
*Input Parameters: unsigned int hour (according to time representation the range can be 0 - 23 or 0 - 12)
*Return Parameter: None
*************************************************************************************************************************************/ 
void RTCInt::setHour(unsigned int hour, unsigned char meridian)
{
  if (time_Mode) {
    if((hour >= 0) && (hour <= 23))
	{	
		timeInt.hour = hour;
	}
	else return;	
  } 
  else {
    if((hour >= 0) && (hour <=12)) 
	{	
		if(meridian)
			hour+=12;
		timeInt.hour= hour;
	}
	else return;	
  }
}

/************************************************************************************************************************************
*Description: Function for setting minute 	
*Input Parameters: unsigned int minute (range 0 - 59)
*Return Parameter: None
*************************************************************************************************************************************/ 
 void RTCInt::setMinute(unsigned int minute)
{
  if((minute >=0) && (minute <= 59))
  {		
	timeInt.minute = minute;
  }
  else return;	
}
 
/************************************************************************************************************************************
*Description: Function for setting second 	
*Input Parameters: unsigned int second (range 0 - 59)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setSecond(unsigned int second)
{
  if((second >=0) && (second <= 59))
  {		
	timeInt.second = second;
  }
  else return;	
}

/************************************************************************************************************************************
*Description: Function for setting time 	
*Input Parameters: unsigned int hour (range 0 - 23 or 0 - 12 according to time representation)
				   unsigned int minute (range 0 - 59)
				   unsigned int second (range 0 - 59)		
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setTime(unsigned int hour,unsigned char meridian, unsigned int minute, unsigned int second)
{
  setHour(hour,meridian);
  setMinute(minute);
  setSecond(second);  
} 

/**********************************************************************************************************************
*Description: Function for setting time (hour, minute and second). This function sets time retrieving values from a 
			  local structure called time accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setTime(void)
{
	setHour(time.hour,time.Tmode);
    setMinute(time.minute);
    setSecond(time.second);  
}

/*
 * Set Date Functions
 */


/************************************************************************************************************************************
*Description: Function for setting day	
*Input Parameters: unsigned int day (range 1 - 31)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setDay(unsigned int day)
{
  if((day >= 1) && (day <=31))
  {
  dateInt.day = day;
  }
  else return;
}

/************************************************************************************************************************************
*Description: Function for setting month 	
*Input Parameters: unsigned int month (range 1 - 12)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setMonth(unsigned int month)
{
  if((month >= 1) && (month <=12))
  {	  
  dateInt.month = month;
  }
  else return;
}

/************************************************************************************************************************************
*Description: Function for setting year 	
*Input Parameters: unsigned int year (note: add offset to this value to obtain correct representation for year)
*Return Parameter: None 
*************************************************************************************************************************************/
void RTCInt::setYear(unsigned int year)
{
  if(year >= 0)
  {	  
  dateInt.year = year;
  }
  else return;
}

/**********************************************************************************************************************
*Description: Function for setting date (day, month and year).	
*Input Parameters: unsigned int day (range 1 - 31)
				   unsigned int month (range 1 - 12)
				   unsigned int year (> 0)		
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setDate(unsigned int day, unsigned int month, unsigned int year)
{
  setDay(day);
  setMonth(month);  
  setYear(year);
}

/**********************************************************************************************************************
*Description: Function for setting date (day, month and year). This function retrieves values from a local structure 
			  called date accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setDate(void)
{
	setDay(date.day);
	setMonth(date.month);  
    setYear(date.year);
}

/*
 * Set Alarm Functions
 */
 

/**********************************************************************************************************************
*Description: Function for enabling alarm.	
*Input Parameters: unsigned int mode:
										 OFF                 alarm disabled
										 SEC                 alarm match on seconds
										 MMSS                alarm match on minutes and seconds
										 HHMMSS              alarm match on hours, minute and seconds
										 DDHHMMSS            alarm match on day, hours, minutes and seconds
										 MMDDHHMMSS          alarm match on month, day hours, minutes and seconds
										 YYMMDDHHMMSS        alarm match on year, month, day, hours, minutes and seconds	
				   unsigned int type:
										 ALARM_POLLED        alarm matching must be monitored from the code
										 ALARM_INTERRUPT     when alarm match occurs will be generated an interrupt
				   
				   voidFuncPtr callback: pointer to interrupt routine for alarm match (write NULL if you use ALARM_POLLED)		
*Return Parameter: None
***********************************************************************************************************************/
 void RTCInt::enableAlarm(unsigned int mode, unsigned int type, voidFuncPtr callback)
 {
	 Alarm_Mode = mode;
	 alarm.type=type;
	 _callback = callback;
 }

 
/**********************************************************************************************************************
*Description: Function for setting alarm. This function retrieves values from two local structures 
			  called date and time accessible in the class RTCInt. According to match alarm type, values 
			  can be obtained from time, date or both structures.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
 void RTCInt::setAlarm(void)
 {
   uint32_t sec=0; 
   uint8_t sum=0;
   uint32_t days=0;
   uint32_t moreTime=0;
   uint8_t noAlarm=0;
   uint8_t alreadySet=0;
   switch(RTCInt::Alarm_Mode)
   {
   case OFF :
      break;
   case YYMMDDHHMMSS:
		//calculate the difference between current date and allarm date
		days = rdn(date.year, date.month, date.day) - rdn(dateInt.year, dateInt.month, dateInt.day);
		if(days<=0) //user is waiting for a past date
			noAlarm=1;
		alarm.sec_to_next_int=-1;
		break;
   case MMDDHHMMSS :
		days = rdn(0, date.month, date.day) - rdn(0, dateInt.month, dateInt.day);
		if(days<=0) //calculate the days to the next year
			days = rdn(dateInt.year+1, date.month, date.day) - rdn(dateInt.year, dateInt.month, dateInt.day);
		if(!(dateInt.year%4)){	
			alarm.sec_to_next_int=366*24*60*60;
			alarm.addDay=1;
		}
		else{
			alarm.sec_to_next_int=365*24*60*60;
			alarm.addDay=(dateInt.year%4)+1;
		}
		break;		
   case DDHHMMSS :
		days = rdn(dateInt.year, date.month, date.day) - rdn(dateInt.year, dateInt.month, dateInt.day);
		if(days<=0) //calculate the days to the next month
			days = rdn(dateInt.year, dateInt.month+1, date.day) - rdn(dateInt.year, dateInt.month, dateInt.day);
		alarm.sec_to_next_int=(rdn(dateInt.year, dateInt.month+2, date.day)-rdn(dateInt.year, dateInt.month+1, date.day))*24*60*60;
		alarm.addMonth=1;
		break;
   case HHMMSS :
		if(time_Mode==TIME_H12 && time.Tmode==POST_MERIDIAN)
			time.hour+=12;

		if(time.hour < timeInt.hour)
			sec=(24-timeInt.hour+time.second)*60*60;
		else
			sec=(time.hour-timeInt.hour)*60*60;	
		alarm.sec_to_next_int=24*60*60;
		alreadySet=1;
		//no break so we keep counting seconds
   case MMSS :
		if(time.minute < timeInt.minute){
			moreTime=(60-timeInt.minute+time.minute)*60;
			sum=0;
		}
		else{
			moreTime=(time.minute-timeInt.minute)*60;
			sum=1;
		}
		//if sec has some value then we need to subtract the already calculated time
		//e.g. between 08:59:50 and 09:00:10 we count one hour in the previous case
		if(sec!=0 /*&& moreTime!=0*/)
			if(sum==0)
				sec-=(3600-moreTime);
			else
				sec+=moreTime;
		else
			sec=moreTime;
		if(!alreadySet){
			alarm.sec_to_next_int=60*60;
			alreadySet=1;
		}
		//no break so we keep counting seconds
   case SEC :
		if (time.second < timeInt.second){
			moreTime=(60-timeInt.second)+time.second;
			sum=0;
		}
		else{
			moreTime=time.second-timeInt.second;
			sum=1;
		}
		//same issue of the above case
		if(sec!=0 /*&& moreTime!=0*/)
			if(sum==0)
				sec-=(60-moreTime);
			else
				sec+=moreTime;		
		else
			sec=moreTime;

		if(!alreadySet)
			alarm.sec_to_next_int=60;
		break;	

   default :
      break;
   }
	  if(!noAlarm){
		sec+=days*24*60*60;
		//cover the case in which user choose the same hour, minute and second
		//in one of the following mode.
		if(sec==0 && Alarm_Mode==HHMMSS)
			sec=24*60*60; //wait for a day
		if(sec==0 && Alarm_Mode==MMSS)
			sec=60*60;	//wait for an hour
		if(sec==0 && Alarm_Mode==SEC)
			sec=60; //wait for a minute
		alarm.seconds=sec;
		nrf_rtc_cc_set(NRF_RTC2, 0, sec*8);
		nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
		nrf_rtc_int_enable(NRF_RTC2, NRF_RTC_INT_COMPARE0_MASK);
		alarm.active=1;
	  }
} 

/**********************************************************************************************************************
*Description: Function for getting status of alarm match. This function is used when you adopt ALARM_POLLED mode 	
*Input Parameters: None
*Return Parameter: bool alarm match
***********************************************************************************************************************/
bool RTCInt::alarmMatch(void)
{
	   return alarm.set;
}

/**********************************************************************************************************************
*Description: Function for clearing the alarm flag.This function is used when you adopt ALARM_POLLED mode 	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::alarmClearFlag(void)
{
	alarm.set=0;
}
 
/**********************************************************************************************************************
*Description: Function for convert a date to integer using Rata Die. Rata Die day one is 0001-01-01.
			  https://en.wikipedia.org/wiki/Rata_Die
			  http://stackoverflow.com/questions/14218894/number-of-days-between-two-dates-c
*Input Parameters: unsigned int y
				   unsigned int m
				   unsigned int d
*Return Parameter: unsigned int
***********************************************************************************************************************/ 
unsigned int RTCInt::rdn(int y, int m, int d) {
    if (m < 3)
        y--, m += 12;
    return 365*y + y/4 - y/100 + y/400 + (153*m - 457)/5 + d - 306;
}
 
 
 
#ifdef __cplusplus
extern "C"{
#endif


void RTC2_IRQHandler(void)
{
    if(nrf_rtc_event_pending(NRF_RTC2, NRF_RTC_EVENT_TICK)){
		nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_TICK);
		// tick every 125 ms --> 8 ticks to get 1 s
		if(++count==8){
			count=0;
			if(++rtcPointer->timeInt.second==60){
				rtcPointer->timeInt.second=0;
				if(++rtcPointer->timeInt.minute==60){
					rtcPointer->timeInt.minute=0;
					if(++rtcPointer->timeInt.hour==24){
						rtcPointer->timeInt.hour=0;
						if(++rtcPointer->dateInt.day==daysPerMonth[rtcPointer->dateInt.month-1][!(rtcPointer->dateInt.year % 4)]){
							rtcPointer->dateInt.day=1;
							if(++rtcPointer->dateInt.month==13){
								rtcPointer->dateInt.month=1;
								rtcPointer->dateInt.year++;
							}
						}
					}
				}
			}
		}
	}
	if(nrf_rtc_event_pending(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0)){
		nrf_rtc_event_clear(NRF_RTC2, NRF_RTC_EVENT_COMPARE_0);
		nrf_rtc_task_trigger(NRF_RTC2, NRF_RTC_TASK_CLEAR);
		nrf_rtc_cc_set(NRF_RTC2, 0, rtcPointer->alarm.sec_to_next_int*8);
		
		//calculate the number of seconds to wait before the next alarm
		if(rtcPointer->alarm.addDay){
			if(rtcPointer->alarm.addDay==5){
				rtcPointer->alarm.sec_to_next_int=366*24*60*60;
				rtcPointer->alarm.addDay=1;
			}
			else
				rtcPointer->alarm.addDay++;
		}
		if(rtcPointer->alarm.addMonth)
			rtcPointer->alarm.sec_to_next_int=(rtcPointer->rdn(rtcPointer->dateInt.year, rtcPointer->dateInt.month+1, rtcPointer->dateInt.day)-rtcPointer->rdn(rtcPointer->dateInt.year, rtcPointer->dateInt.month, rtcPointer->dateInt.day))*24*60*60;
		rtcPointer->alarm.seconds=rtcPointer->alarm.sec_to_next_int;
		
		if(rtcPointer->alarm.type==ALARM_POLLED)
			rtcPointer->alarm.set=1;
		else
			if(_callback!=NULL)
				_callback();
	}
}
		
		
#ifdef __cplusplus
}
#endif


	