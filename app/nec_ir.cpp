#include <nec_ir.h>
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#define SERIAL_BAUD_RATE 115200
Timer procTimer;
 
IR_rec::IR_rec(){
    
}
/**
 * Interupt handler tahts called when edge is found
 */
void IRAM_ATTR IR_rec::IRinterruptHandler() {
    unsigned long tim;
    uint8 upperB;
    uint8 lowerB;
    switch (irState) {
        case IDLE:
            irStartTime = micros(); //first edge so start timing
            idleTime=micros(); //used to detect when stuck in state
            irState = START;
            bitCnt=0;
            break; 
        case START: //gets stuck here because it doesn't get reset to idle on last pulse /
                    //should only move to start after long pulse
                    // or set time out for this state (and others)
            tim = micros() - irStartTime; //should be around 13.5mS
            irStartTime = micros();
            if (tim>10500 && tim <12000 ) //should be 11250 uS
            {
              irState = REPEAT;    
              break;
            }
            if (tim < 13000 || tim > 14000) { //TODO implement Repeat code
                irState = IDLE; //Reject the start bit and wait for valid one

            } else {
                irState = RUN;
                irData = 0;
                irAddr = 0;
            }
            break;

        case RUN:
            tim = micros() - irStartTime; //should be around 1.125 for 0 and 2.25 for a 1
            irStartTime = micros();
            if (tim > 3000) {
                irState = IDLE; //Invalid time
                bitCnt = 0;
        
                break;
            }
            if (bitCnt > 15) {
                irData = irData * 2;
                if (tim > 1750) { //TODO implement Repeat code
                    irData = irData + 1; //make LSB 1
                }
            } else {
                irAddr = irAddr * 2;
                if (tim > 1750) { //TODO implement Repeat code
                    irAddr = irAddr + 1; //make LSB 1
                }
            }

            bitTimes[bitCnt] = tim;
            bitCnt++;
            if (bitCnt >= NUMBITS) {
                    //check the data the upper byte should be the inverse of the lower
                     lowerB = irData & 0xFF;
                     upperB = (irData >> 8)^0xff;
                     if(lowerB==upperB) {
                       irState = CAPTURE;
                       
                     }
                     else{
                        irState =IDLE;  
                         //debug
                    
                     }
                    
                bitCnt = 0;
            }
            break;
        case REPEAT:
             irRepeat=irData;
             irState =CAPTURE;  
        default:
            break;
    }

}
/**
 * required to start the capture
 */
void IR_rec::resetIR() {
    irState = IDLE; //wait for start pulse
    bitCnt = 0;
    irRepeat =0;
}

/**it's possible to get stuck in a state 
if we see the state is Not idle we can check for time and reset if required */
void IR_rec::checkForStuckState(){
    if(irState==IDLE) return;
    if(irState==CAPTURE) return; //could reset if in Capture too long?
    if((idleTime-micros())>200000 ){ //200mS should be finished by now
    resetIR();
     //   Serial.printf("Reset");  
    }
}



/**
 * For Debug , dumps the raw times for each bit
 */
void IR_rec::dumpIRTimes() {
    int i = 0;
    for (i = 0; i < NUMBITS; i++) {
        Serial.printf("%d = %d\n", i, bitTimes[i]);
    }
}

//key 4ab5key 6897key 9867key b04fkey 30cfkey 18e7key 7a85key 10efkey 38c7key 5aa5key 42bdkey 52adkey 22ddkey a857key c23dkey 629dkey 2fd
/**
 * decodes the captured data 
 * @return char representing the button press
 */
char IR_rec::decodeIR() {
    if (irState != CAPTURE) return 0; //can't decode as no captured data
    switch (irData) {
        case 0x4ab5: //0
            return '0';
            break;
        case 0x6897: //1
            return '1';
            break;
        case 0x9867: //2
            return '2';
            break;
        case 0xb04f: //3
            return '3';
            break;
        case 0x30cf:  //4
            return '4';
            break;
        case 0x18e7:  //5
            return '5';
            break;
        case 0x7a85:  //6
            return '6';
            break;
        case 0x10ef:  //7
            return '7';
            break;
        case 0x38c7:  //8
            return '8';
            break;
        case 0x5aa5:  //9
            return '9';
            break;
        case 0x42bd:  //*
            return '*';
            break;
        case 0x52ad:  //#
            return '#';
            break;
        case 0x22dd:  //<
            return '<';
            break;
         case 0xa857:  //d
             return 'd';
            break;   
         case 0xc23d:  //>
             return '>';
            break;   
         case 0x629d:  //u
             return 'u';
            break;
         case 0x02fd:  //ok
             return 'o';
            break;   
        default:
            return '?';
            break;
    }
    return '?';
}

/**
 * sets the callback function which should tack a char parameter i.e. the key
 * @param callback
 */
void IR_rec::setIRCallBack(IRCallback callback){
    recCallback=callback;
       recDelegateCallback=null;
}
/**
 * sets the Delegate callback function which should tack a char parameter i.e. the key
 * @param callback
 */
void IR_rec::setIRCallBack(IRRecDelegate callback){
    recDelegateCallback=callback;
    recCallback=null;
}

//TODO make the delegate version

/***
 * checks the state to see if there is a key press
 */
void IR_rec::checkIR() {
    if (irState == CAPTURE) {
        char c= decodeIR();
        char r='x';
        if(irRepeat){
            r='r';
        }
        
        if(c){
          //test to see which callback to use  
          if(recDelegateCallback){
              recDelegateCallback(c);
          } 
          else if(recCallback) {
           recCallback(c);
          }
        // Serial.printf("key %c repeat %c\n",c,r);
        }
        else{
           Serial.printf("Something bad here");  
        }
        //  dumpIRTimes();
        resetIR();
    }
   checkForStuckState();
}

/**
 * Start the IR receiver 
 * @param irpin = Pin that the receiver module is connected to
 */
void IR_rec::start(int irpin) {
   // Serial.begin(SERIAL_BAUD_RATE); // 115200 or 9600 by default
  //  Serial.println("Setting up IR");
    pinMode(irpin, INPUT);
     attachInterrupt(irpin, IRHandlerDelegate(&IR_rec::IRinterruptHandler,this), FALLING);
    resetIR();
     procTimer.initializeMs(500, TimerDelegate(&IR_rec::checkIR,this)).start(); // every 500 milli seconds
    //Serial.println("IR SETUP, waiting on key press");
}

