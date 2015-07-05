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
    ir.start(IR_PIN);
}