#ifndef STMR_h
#define STMR_h

#include <stdint.h>
#include "AFMR.h"
#include "HMII.h"
#include "TIMR.h"


enum STMR_States{
  Initialization = 0,
  Waiting,               //1
  Starting,              //2
  NonContinuousRunning,  //3
  ContinuousTransition,  //4
  ContinuousRunning,     //5
  Ending,                //6
  Erroring, 
};

void STMR_init();

extern uint8_t STMR_StateMachine();

extern void STMR_State_Update(uint8_t state);

#endif