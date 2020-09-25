 /*!
  * file get_host_electrical_parameters.ino
  *
  *Stack the two coils, connect the IIC wire, and run the routine
  *Serial port prints the data sent by the slave，Get the electrical parameters of the slave
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

uint8_t rxbuf[10];
String str="";
uint8_t bufsize=4;
uint8_t txbuf[4]={0x11,0x22,0x33,0x44};

CWWIRELESS_IIC CWWIRELESS(0x24,SLAVE);

void setup()
{
  Serial.begin(115200);
  CWWIRELESS.slaveStatus();
  Serial.print("coil:\t");
  Serial.println(sSlavemessage.coil);
  Serial.print("Vin:\t");
  Serial.print(sSlavemessage.Vin);
  Serial.println(" mV");
  Serial.print("Iin:\t");
  Serial.print(sSlavemessage.Iin);
  Serial.println(" mA");
  Serial.print("RcvPwr:\t");
  Serial.print(sSlavemessage.RcvPwr);
  Serial.println(" mW");
  Serial.print("Temp:\t");
  Serial.println(sSlavemessage.Temp);
  Serial.print("Fre:\t");
  Serial.println(sSlavemessage.Fre);
  Serial.print("Duty:\t");
  Serial.println(sSlavemessage.Duty);
  Serial.print("NTC:\t");
  Serial.println(sSlavemessage.NTC);
}

void loop()
{
  CWWIRELESS.getSlavePPPPdata(rxbuf,bufsize);
  Serial.print(str+"Get HOST "+ bufsize +" messages["); 
  for(int i = 0;i< bufsize; i++)
  {
    Serial.print("0x");
    Serial.print(rxbuf[i],HEX);
    Serial.print(" ");
  }
  Serial.println("]");
  CWWIRELESS.setSlavePPPPdata(txbuf,bufsize);
  delay(2000);
}