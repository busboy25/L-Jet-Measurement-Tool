#include "STMR.h"

#define DISABL 0
#define ENABL 1 

struct _STMR_Reg{
  uint8_t state : 4;
  uint8_t error : 1;
  uint8_t :3;
} STMR_Reg;


void STMR_init(){
  STMR_Reg.state = Initialization;
  STMR_Reg.error = 0;
}

void STMR_Variable_Clear(){
  //IGN
  TIMR_IGN_Timer_Update(0, 0);
  //INJ
  TIMR_INJ_Timer_Update(0, 0);
  //AFM
  ARMR_AFM_Update(0);
  //BATT
  TIMR_BATT_CC_Update(0);
  //TEMP
  TIMR_TEMP_CC_Update(0);
  //RUNNING
  digitalWrite(PINC_RUNNING, 0);
}

//function works, tested on 10/6
void STMR_Test_Enable(bool state){
    AFMR_DAC_Enable(state);
    TIMR_TCC0_Enable(state);
    TIMR_TCC1_Enable(state);
    TIMR_TC4_Enable(state);
    if(!state){
      digitalWrite(PINC_RUNNING, LOW);
    }
}


extern uint8_t STMR_StateMachine(){
  //Serial.println("State Machine");
  if(STMR_Reg.error){
    STMR_Reg.state = Erroring;
  }
  switch(STMR_Reg.state){
    case Initialization:
      //Serial.println("Initialization"); //This state is entered without init of the state, test on 10/6
      HMIO_LED_Enable(1,0,0);
      STMR_Reg.state = Waiting;
    case Waiting:
      //Serial.println("Waiting"); //This state is entered without init of the state, test on 10/6
      if(! HMII_UserButton()){
        break;
      }
      STMR_Reg.state = Starting;
    case Starting:
      Serial.println("Starting"); //This state is entered without init of the state after a user button push from waiting, test on 10/6
      HMIO_LED_Enable(1,1,0);
        //Start the test
        //Sent TSTR Setup the value read from the test switches
        //within setup the test pointer is set to the test profile
        //and the length of the test is noted
      //Serial.println(HMII_TestRead(), BIN);
      TSTR_Setup(HMII_TestRead()); 
      //Initialize the time value to -1 so then it can be updated
      //to 0 during the first running of the 1ms time task
      STMR_Test_Enable(ENABL); //Enable outputs, functional tested 10/6
      STMR_Reg.state = NonContinuousRunning;
      break;
    case NonContinuousRunning:
      //Serial.println("Non Continuous Running"); //This state is entered without init of the state from starting, test on 10/6
        /*On a 1ms task
        //Update the test time (even the first time this is good 
        //because the time is initialized at -1 during setup)
        //
        //Check the value of the test time to see
        //if it equals the time of the next test row 
        //  check the value of the test time to see
        //  if it equals to the continuous time value
        //End the test if the test is at the end of the run.
        //Put the test into continuous run if the current time 
        //equals the future value and equals the continuous time value.
        //If neither of these things happen update the
        */
      
      if(!HMII_UserButton()){
        break;
      }
      STMR_Reg.state = Ending;
      break;
    case ContinuousTransition:
      Serial.println(F("Continuous Transition"));
      HMIO_LED_Enable(1,1,1);
      //May need to set output values or can this be done in 
      //Non Continuous before ending
      STMR_Reg.state = ContinuousRunning;
    case ContinuousRunning:
      Serial.println(F("Continuous Running"));
      if(!HMII_UserButton()){
        break;
      }
      STMR_Reg.state = Ending;
    case Ending:
      STMR_Test_Enable(DISABL); //Disable outputs, functional tested 10/6
      HMIO_LED_Enable(1,0,0);   
      STMR_Variable_Clear(); //Clear output buffers (CCx, PER, DAC, GPIO)
      STMR_Reg.state = Waiting;
      Serial.println("Ended"); //This state is entered without init of the state after a user button push, test on 10/6
      break;
    case Erroring:
      Serial.println(F("Erroring"));
      STMR_Test_Enable(DISABL); //Disable outputs, functional tested 10/6
      HMIO_LED_Enable(0,0,0); //All LED off in erroring
      while(1){}; //Do nothing
      break;
  } 
  return STMR_Reg.state;
}

extern void STMR_State_Update(uint8_t state){
  STMR_Reg.state = state;
}

