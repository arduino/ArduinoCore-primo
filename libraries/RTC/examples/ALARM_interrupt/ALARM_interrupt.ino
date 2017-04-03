/*****************************************************************************************************************************************************************************
* This sketch demonstrate how to use alarm in interrupt mode.
This mode is more conveniently because you use processor for other tasks and when alarm match occurs interrupt routine is executed.
In this way, alarm flag checking is indipendent from main program flow.

NOTE: for M0/M0 pro only you can select the oscillator source for count.
If you want to use a low power oscillator use rtc.begin(TIME_H24, LOW_POWER); function.
If you want to use a more accurate oscillator use rtc.begin(TIME_H24, HIGH_PRECISION); function.
******************************************************************************************************************************************************************************/


#include <RTCInt.h>

RTCInt rtc;

void setup() 
{
  Serial.begin(9600);       //serial communication initializing
  pinMode(LED_BUILTIN,OUTPUT);
  rtc.begin(TIME_H24);      //RTC initializing with 24 hour representation mode
  rtc.setTime(17,0,5,0);    //setting time (hour minute and second)
  rtc.setDate(13,8,15);     //setting date
  rtc.enableAlarm(SEC,ALARM_INTERRUPT,alarm_int); //enabling alarm in polled mode and match on second
  rtc.time.hour=17;
  rtc.time.minute=5;
  rtc.time.second=10;  //setting second to match
  rtc.setAlarm();  //write second in alarm register
}

void loop()
{
  digitalWrite(LED_BUILTIN,HIGH);   //main program code
  delay(100);
  digitalWrite(LED_BUILTIN,LOW);
  delay(900);
  
}


/*************** Interrupt routine for alarm ******************************/
void alarm_int(void)
{
  Serial.println("Alarm match!");
    for(int i=0; i < 10; i++)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      //delay(200);
      for(int j=0; j < 1000000; j++) asm("NOP");  //in interrupt routine you cannot use delay function then an alternative is NOP instruction cicled many time as you need
      digitalWrite(LED_BUILTIN,LOW);
      //delay(800);
      for(int j=0; j < 2000000; j++) asm("NOP");
    }
    rtc.alarmClearFlag();  //clearing alarm flag
}    
