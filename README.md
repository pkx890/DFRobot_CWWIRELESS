# CWWIRELESS
This library is used to communicate between two wireless charging modules

## DFRobot_CWWIRELESS Library for Arduino
---------------------------------------------------------

## Table of Contents

* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [Credits](#credits)

<snippet>
<content>

## Installation

To use this library, download the zip file and extract it into a folder called DFRobot_CWWIRELESS.

## Methods

```C++

  /**
   * @Set the callback function
   */  
  void (*callback)();

  /**
   * @Put the string into the host queue
   */  
  void transferHoststring(char* str);
  
  /**
   * @Loop through the transceiver function
   */  
  void loop();
  
  /**
   * @Put the string into the slave queue
   */  
  void transferSlavestring(char* str);
  
  /**
   * @Receive host string
   */  
  String receiveHoststring();
  
  /**
   * @Send a packet from the slave to the host
   */  
  void slaveBegintransfer();
  
  /**
   * @@Send a packet from the host to the slave
   */  
  void hostBegintransfer();
  
  /**
   * @Receive slave string
   */  
  String reciveSlavestring();

```  

## Compatibility  

MCU                | Work Well | Work Wrong | Untested  | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino uno |       √      |             |            | 
  
## Credits
Written by Peng Kaixing(kaixing.peng@dfrobot.com), 2020. (Welcome to our website)
