# nec_ir_rec
esp8266 IR receiver (nec protocol) using Sming framework

this uses the sming framework https://github.com/anakod/Sming
Sming now has a ported vbersion of the Ardiuno IR library, the Ardiuno library is more capable but also uses more resourses. 
This library only supports the NEC proptocol which is used my may ebay modules
e.g. http://www.ebay.co.uk/itm/301435006235?_trksid=p2060353.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT

This library uses a callback when the IR code has been detected, so there is no need to poll, it's very eay to use.
Note There is no transmit just receive.

example usage
```C++
#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "nec_ir.h"
#define IR_PIN 13   // GPIO13
IR_rec ir=IR_rec();

void printIR(char c){
    Serial.print(c);
}

void init()
{
    ir.setIRCallBack(printIR);
    ir. start(IR_PIN);
}
```
