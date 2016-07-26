/**********************************************************************************************************************************************************
This sketch gives a simple demonstration of how to use RTC library.
The code sets date and time using internal structure and then print on serial date and time. Time representation is 24 hour mode

NOTE: for M0/M0 pro only you can select the oscillator source for count.
If you want to use a low power oscillator use rtc.begin(TIME_H24, LOW_POWER); function.
If you want to use a more accurate oscillator use rtc.begin(TIME_H24, HIGH_PRECISION); function.
**********************************************************************************************************************************************************/



#include <RTCInt.h>

RTCInt rtc;  //create an RTCInt type object


void setup() 
{
  Serial.begin(9600);
  rtc.begin(TIME_H24); //init RTC in 12 hour mode
  
  //time settings
  rtc.setHour(15,0);  //setting hour
  rtc.setMinute(43);  //setting minute
  rtc.setSecond(0);   //setting second
  
  
  rtc.setDay(13);     //setting day
  rtc.setMonth(8);    //setting month
  rtc.setYear(15);    //setting year
  
}

void loop() 
{
 rtc.getDate();      //getting date in local structure (local_date)
 rtc.getTime();      //getting time in local structure(local_time)
 
 //printing date in format YYYY/MM/DD
 Serial.print(rtc.date.year+2000); // year
 Serial.print('/');
 Serial.print(rtc.date.month);    // month
 Serial.print('/');
 Serial.print(rtc.date.day);      // day
 Serial.print(' ');
 
 //printing time
 Serial.print(rtc.time.hour);    //hour
 Serial.print(':');
 Serial.print(rtc.time.minute);  //minute
 Serial.print(':');
 Serial.println(rtc.time.second);  //second
 
 delay(1000);
}
