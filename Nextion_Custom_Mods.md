Title: Nextion Arduino Library Customizations

# Nextion Arduino Library Customizations
All customizations created by different authors, they are credited below.

The Nextion Display has an official Arduino Library: <https://github.com/itead/ITEADLIB_Arduino_Nextion>

This library is a little dated (last updated in Jan 2017, mainly just the HMI examples)

I needed some additional functionality as described below:
## NexConfig.h
__Author:  Will Travis__

changed the dbSerialPrint to accept multiple variables,  e.g. dbSerialPrint(variable, HEX)
```
FROM:
    #define dbSerialPrint(a)    dbSerial.print(a)
    #define dbSerialPrintln(a)  dbSerial.println(a)
    #define dbSerialBegin(a)    dbSerial.begin(a)
TO:
    #define dbSerialPrint(...)    dbSerial.print(__VA_ARGS__)
    #define dbSerialPrintln(...)  dbSerial.println(__VA_ARGS__)
    #define dbSerialBegin(...)    dbSerial.begin(__VA_ARGS__)
```
## NexHardware.cpp
__Author:  Will Travis__

Primarily removed the goto statements and added additional error information

### _recRetNumber_ and _recvRetCommandFinished_ functions
* For example added this error message: `dbSerialPrintln("recvRetNumber ERROR: readBytes != length of 8");`
instead of the non descriptive original message: `dbSerialPrintln("recvRetNumber err");`
* removed the `goto __return;` this is not good programming and was not needed.  

## NexText.cpp & NexText.h
__Author: indev2__

Added _NexText::setFloatText_ function as defined in <http://support.iteadstudio.com/support/discussions/topics/11000011410> forum topic. 

## NexScreen.h & NexScreen.cpp
__Author: kosza (email: <koszab@gmail.com>)__

New Class with several additional functions including:
* setBrightness
* Sleep
* Wakeup
* setAutoWakeup

See _NexScreen.h_ file for more information

