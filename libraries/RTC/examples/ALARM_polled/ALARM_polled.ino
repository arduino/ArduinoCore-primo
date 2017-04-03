/*********************************************************************************************************************************************************************************
* This sketch demonstrates how configure alarm in polled mode.
  In this mode you hav to check continuously ALARM flag in the main program code.
  
  NOTE: for M0/M0 pro only you can select the oscillator source for count.
If you want to use a low power oscillator use rtc.begin(TIME_H24, LOW_POWER); function.
If you want to use a more accurate oscillator use rtc.begin(TIME_H24, HIGH_PRECISION); function.
*********************************************************************************************************************************************************************************/  


#include <RTCInt.h>

RTCInt rtc;

void setup() 
{
  Serial.begin(9600);       //serial communication initializing
  pinMode(LED_BUILTIN,OUTPUT);
  rtc.begin(TIME_H24);      //RTC initializing with 24 hour representation mode
  rtc.setTime(17,0,5,0);    //setting time (hour minute and second)
  rtc.setDate(13,8,15);     //setting date
  rtc.enableAlarm(SEC,ALARM_POLLED,NULL); //enabling alarm in polled mode and match on second
  rtc.time.hour=17;
  rtc.time.minute=5;
  rtc.time.second=10;  //setting second to match
  rtc.setAlarm();  //write second in alarm register
}

void loop()
{

   digitalWrite(LED_BUILTIN,HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN,LOW);
   delay(400);
   
  if(rtc.alarmMatch())        //when match occurs led on pin 13 blinks ten times
  {
    Serial.println("Alarm match!");
    for(int i=0; i < 10; i++)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN,LOW);
      delay(800);
    }
    rtc.alarmClearFlag(); //clearing alarm flag
  } 
   
}
