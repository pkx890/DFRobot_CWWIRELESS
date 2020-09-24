 /*!
  * file slave.ino
  *
  *Stack the two coils, connect the IIC wire, and run the routine
  *Serial port prints the data sent by the slave
  *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author      PengKaixing(kaixing.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-09-24
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_PAJ7620U2  */
 
#include "DFRobot_CWWIRELESS.h"
#define SLAVE 0
#define HOST 1

CWWIRELESS_IIC CWWIRELESS(0x24,HOST);
String str="";
String cmd1="open_LED";
String cmd2="close_LED";
static long long stime=0; 
static long long rtime=0;
void recvCallback();
void recvCallback(){ 
  if((millis()-rtime)>500){
    if(CWWIRELESS.cumsgBufHead!=NULL){
      CWWIRELESS.hostBegintransfer();
    }
    String str=CWWIRELESS.reciveSlavestring();
    if(str!=""){
      Serial.print(str);
    } 
    rtime = millis();
  }else{
  }
}

void setup()
{
  Serial.begin(115200);
  CWWIRELESS.setCallback(recvCallback);
  CWWIRELESS.transferHoststring("host begin\r\n");
}

void loop()
{ 
  if((millis()-stime)>5000)
  {
    CWWIRELESS.transferHoststring("HELLO WORLD nihao aaa bbb 123 !@$#\r\n");
    stime=millis();
  }
  CWWIRELESS.loop(); 

}
