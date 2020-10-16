 /*!
  * file get_host_electrical_parameters.ino
  *
  *Stack the two coils, connect the IIC wire, and run the routine
  *Serial port prints the data sent by the host and Get message from host
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

uint8_t txbuf[5]={0x48,0x22,0x33,0x44,0x55};
uint8_t bufsize = 5;
uint8_t rxbuf[10];
String str="";

CWWIRELESS_IIC CWWIRELESS(0x24,HOST);

void setup()
{
  Serial.begin(115200);
  Serial.print("showhostChipID:\t0x");
  Serial.println(CWWIRELESS.hostChipID(),HEX);

  Serial.print("showhostVout:\t");
  Serial.print(CWWIRELESS.hostVout());
  Serial.println("\tmV");

  Serial.print("showhostFreq:");
  Serial.print(CWWIRELESS.hostFreq());
  Serial.println("\tHZ");

}

void loop()
{
  CWWIRELESS.sendHostpacket(txbuf,bufsize);
  if(CWWIRELESS.getHostflag())
  {
    uint8_t rsize=CWWIRELESS.receiveHostpacket(rxbuf);
    if(rsize+1 >= 1 )
    {
      Serial.print(str+"Size:"+rsize+" FSK:[H-"+rxbuf[0]+", MSG-");
      for(int i = 1;i < rsize+1;i++)
      {
        Serial.print("0x");
        Serial.print(rxbuf[i+1],HEX);
        Serial.print(" ");
      }
      Serial.println("] ");
     }else{
        Serial.println("FSK: None ");
     } 
  }
}