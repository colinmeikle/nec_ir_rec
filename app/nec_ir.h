/* 
 * File:   nec_ir.h
 * Author: colinm
 *
 * Created on 01 July 2015, 21:21
 */

#ifndef NEC_IR_H
#define	NEC_IR_H
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Delegate.h>


#define IDLE 0
//Line gone low
#define START 1
//valid start time found
#define RUN 2
//code detected but not read
#define CAPTURE 3
//repeat code found
#define REPEAT 4
#define NUMBITS 32

//Syntax Delegate<void( arguments for the function being passed we have none in this case)> 
//We therefore need a deligate type for each function as they may have different arguments
//take case some have been defined check the headers
typedef Delegate<void()> IRHandlerDelegate;
typedef Delegate<void(char c)> IRRecDelegate;
//regular callback
typedef void (*IRCallback)(char);
class IR_rec {
    //waiting on a start pulse

public:
    IR_rec();
    /** IR code before it's decoded*/
    volatile uint16 irData;
    /** Set if the code is repeated I.e. key held*/
    volatile uint16 irRepeat;
    /** Address of the IR code*/
    volatile uint16 irAddr;
    /** state of the state machine*/
    volatile uint16 irState;
    void setIRCallBack(IRCallback callback=null);
    void setIRCallBack(IRRecDelegate callback=null);
    void dumpIRTimes() ;
    void start(int irpin) ;

private:
    IRCallback recCallback=null;
    volatile unsigned long irStartTime ; //
    volatile unsigned long idleTime ;
    volatile uint16 irPulseTime ; //length of pulse in uS
    volatile int bitCnt ;
    volatile uint16 bitTimes[NUMBITS]; //there are 32 bits in the tx data
    void IRAM_ATTR IRinterruptHandler();
    void resetIR() ;
    IRRecDelegate recDelegateCallback;
    void checkForStuckState();
    char decodeIR() ;
    void checkIR();
   

};

#endif	/* NEC_IR_H */

