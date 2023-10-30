#include <Arduino.h>
#include <stdint.h>
#include "TIMR.h"
#include "TSTR.h"
#include "STMR.h"

#define TSTRDebug0  0
#define TSTRDebug1  0
#define TSTRDebug2  0
#define TSTRDebug3  0
#define TSTRDebug4  0
#define TSTRDebug5  0
#define NumOfTest   16

unsigned long* TSTR_time_prev_ptr;

unsigned long TSTR_Test_Profile_Len[NumOfTest]={
  sizeof(TSTR_TestProfile_0x0)/sizeof(TSTR_TestProfile_0x0[0]),
  sizeof(TSTR_TestProfile_0x1)/sizeof(TSTR_TestProfile_0x1[0]),
  sizeof(TSTR_TestProfile_0x2)/sizeof(TSTR_TestProfile_0x2[0]),
  sizeof(TSTR_TestProfile_0x3)/sizeof(TSTR_TestProfile_0x3[0]),
  sizeof(TSTR_TestProfile_0x4)/sizeof(TSTR_TestProfile_0x4[0]),
  sizeof(TSTR_TestProfile_0x5)/sizeof(TSTR_TestProfile_0x5[0]),
  sizeof(TSTR_TestProfile_0x6)/sizeof(TSTR_TestProfile_0x6[0]),
  0,//sizeof(TSTR_TestProfile_0x7)/sizeof(TSTR_TestProfile_0x7[0]),
  0,//sizeof(TSTR_TestProfile_0x8)/sizeof(TSTR_TestProfile_0x8[0]),
  0,//sizeof(TSTR_TestProfile_0x9)/sizeof(TSTR_TestProfile_0x9[0]),
  0,//sizeof(TSTR_TestProfile_0xA)/sizeof(TSTR_TestProfile_0xA[0]),
  0,//sizeof(TSTR_TestProfile_0xB)/sizeof(TSTR_TestProfile_0xB[0]),
  0,//sizeof(TSTR_TestProfile_0xC)/sizeof(TSTR_TestProfile_0xC[0]),
  0,//sizeof(TSTR_TestProfile_0xD)/sizeof(TSTR_TestProfile_0xD[0]),
  sizeof(TSTR_TestProfile_0xE)/sizeof(TSTR_TestProfile_0xE[0]),
  sizeof(TSTR_TestProfile_0xF)/sizeof(TSTR_TestProfile_0xF[0])
};

static struct _TSTR_Test{
  long            time;
  unsigned long   position;
  unsigned long   length;
  int             test;
} TSTR_Test;

unsigned long* TSTR_TestProfiles[NumOfTest] = {
  &TSTR_TestProfile_0x0[0][0],
  &TSTR_TestProfile_0x1[0][0],
  &TSTR_TestProfile_0x2[0][0],
  &TSTR_TestProfile_0x3[0][0],
  &TSTR_TestProfile_0x4[0][0],
  &TSTR_TestProfile_0x5[0][0],
  &TSTR_TestProfile_0x6[0][0],
  0,//&TSTR_TestProfile_0x7,
  0,//&TSTR_TestProfile_0x8,
  0,//&TSTR_TestProfile_0x9,
  0,//&TSTR_TestProfile_0xA,
  0,//&TSTR_TestProfile_0xB,
  0,//&TSTR_TestProfile_0xC[0][0],
  0,//&TSTR_TestProfile_0xD[0][0],
  &TSTR_TestProfile_0xE[0][0],
  &TSTR_TestProfile_0xF[0][0]
};

extern void TSTR_Setup(uint8_t testProfile){
  
  TSTR_time_prev_ptr = TSTR_TestProfiles[testProfile];

  //Initialize the time value to -1 so then it can be updated
  //to 0 during the first running of the 1ms time task
  TSTR_Test.time      = -1;
  TSTR_Test.position  = 1;
  TSTR_Test.length    = TSTR_Test_Profile_Len[testProfile];
  TSTR_Test.test      = (int) testProfile;

  TIMR_INJ_Timer_Update(*(TSTR_time_prev_ptr + InjPeriod), 0);
  TIMR_IGN_Timer_Update(*(TSTR_time_prev_ptr + IgnPeriod), 0);
  TIMR_BATT_CC_Update(0);
  TIMR_TEMP_CC_Update(0);
  //TIMR_BATT_CC_Update(*(TSTR_time_prev_ptr + BATT));
  //TIMR_TEMP_CC_Update(*(TSTR_time_prev_ptr + TEMP));
  ARMR_AFM_Update(*(TSTR_time_prev_ptr + AFM));

  if(TSTRDebug0){
    Serial.println(F("TSTR Setup"));
    Serial.println(F("Address Start of Test Array ="));
    Serial.println((long)TSTR_time_prev_ptr);
    Serial.println(*TSTR_time_prev_ptr);
    Serial.println(F("Start of Time, Position, Length, Test="));
    Serial.println(TSTR_Test.time);
    Serial.println(TSTR_Test.position);
    Serial.println(TSTR_Test.length);
    Serial.println(TSTR_Test.test);
    Serial.println(F("End TSTR Setup"));
  }
  if(TSTRDebug5){
    Serial.println(TSTR_Test.length);
  }
}

unsigned long TSTR_Linear_Variable_Update(unsigned long t, unsigned long variable){
  //All this pointer work is because standard arrey access of [x][y][z] doesn't work
  //this is because the while it is know that is 16 test (x) and 9 column/variables [z],
  //the number of rows (y) is variable with some test profiles only have 4 row and others have thousands
  //Arrays are just an abstraction of pointers where the accessing the Z position values can be done by 
  //adding the number of address of that variable from the start of the row, the y position can be increased
  //the address by the total number of column in the row. This gives you the address of the first column 
  //value of the next row. We do no use a x array page and rather use a pointer array to store the address
  //of the first value in which of the test profiles. Which of the test profile is use does not change within
  //the running state, and only defined when starting the test.

  if(TSTRDebug3){
    Serial.println("Linear Update Start");
  }

  //unsigned long *time_future, *variable_future, *variable_prev;

  //The value of the next time step is at the adrress of the previous row, defined by TST_time_prev_ptr
  //plus the legnth of the row, defined by TSTR_VariableNum 
  unsigned long* time_future = TSTR_time_prev_ptr + TSTR_VariableNum;
  //The value of the variable is at the address of the next row, by adding TSTR_VaribleNum
  //plus the number of bytes for that variable column, defined by variable
  unsigned long* variable_future = TSTR_time_prev_ptr + TSTR_VariableNum + variable;
  //The value of the variable is at the address prevous row
  //plus the number of bytes for that variable column, defined by variable
  unsigned long* variable_prev = TSTR_time_prev_ptr + variable;

  //Serial.println(TSTR_time_prev_ptr);
  //Serial.println(*TSTR_time_prev_ptr);

  if(TSTRDebug3){
    Serial.println("Linear Update Pointer Update");
  }
  //Serial.println(t);
  //Serial.println(time_future);
  //Serial.println(*time_future);

  if(t == *time_future){
    return *variable_future;
  }
  return map(t, *TSTR_time_prev_ptr, *time_future, *variable_prev, *variable_future);
}

extern uint8_t TSTR_Time_Update(){
  if(TSTRDebug1){
    Serial.println(F("Time Update"));
  }
  unsigned long* time_future;
  time_future = TSTR_time_prev_ptr + TSTR_VariableNum;

  if(TSTRDebug1){
    Serial.println(F("Time Future & Address ="));
    Serial.println(*time_future);
    Serial.println((long) time_future);
  }

  //Check for continuous
  //The first time this is true is ths first time the second to last
  //row is selected because the position and time values have not been updated.
  //Doing nothing after entering CONTINUOSUS TRANSITION will continue to use
  //all the previous buffer values.
  if(CONTINUOSUS_TIME_VALUE == *time_future){
    if(TSTRDebug1){
      Serial.println(F("TSTR_Con_Trans"));
    }
    return ContinuousTransition;
  }

  if(TSTRDebug1){
    Serial.println(F("Time Update 2"));
  }

  //check for ending and increase in position
  TSTR_Test.time++;
  if(TSTRDebug1){
    Serial.println(F("Test Time ="));
    Serial.println(TSTR_Test.time);
  }

  if(TSTR_Test.time == *time_future){
    if(TSTRDebug4){
    Serial.println(F("Time = Time Future"));
    }
    TSTR_time_prev_ptr = time_future;
    TSTR_Test.position ++;
    if(TSTRDebug4){
      Serial.println(F("Before ending"));
    }
    if(TSTR_Test.length == TSTR_Test.position){
      if(TSTRDebug5){
        Serial.println(F("Test Ending"));
      }
      return Ending;
    }
  }

  if(TSTRDebug1){
    Serial.println("Test Time =");
    Serial.println(TSTR_Test.time);
  }

  return NonContinuousRunning;
}

extern void TSTR_All_Variable_Update(){
  //IGN
  
  //The buffer is used because the TC timer does have a hardware buffer and this buffere is loaded on every TC TOP MATCH interrupt
  uint16_t period = limit(TSTR_Linear_Variable_Update(TSTR_Test.time, IgnPeriod), IGN_PER_MAX, IGN_PER_MIN);
  uint16_t pw     = limit(TSTR_Linear_Variable_Update(TSTR_Test.time, IgnDwell), period, IGN_PW_MIN);
  if(period > ignRpmTooLow){
    pw = 0;
  }
  TIMR_IGN_Buffer_Update(period, pw);

  if(TSTRDebug2){
    Serial.println("IGN");
    Serial.println(TIMR_IGN_Value_Buffer[0]);
    Serial.println(TIMR_IGN_Value_Buffer[1]);
  }
  //INJ
  //The TCC hardware buffer is used and thus a interrupt is not needed to be used.
  TIMR_INJ_Buffer_Update(TSTR_Linear_Variable_Update(TSTR_Test.time, InjPeriod), TSTR_Linear_Variable_Update(TSTR_Test.time, InjPW));
  //AFM
  ARMR_AFM_Update(TSTR_Linear_Variable_Update(TSTR_Test.time, AFM));
  //BATT
  //The TCC hardware buffer is used and thus a interrupt is not needed to be used.
  TIMR_BATT_CCB_Update(TSTR_Linear_Variable_Update(TSTR_Test.time, BATT));
  //TEMP
  //The TCC hardware buffer is used and thus a interrupt is not needed to be used.
  TIMR_TEMP_CCB_Update(TSTR_Linear_Variable_Update(TSTR_Test.time, TEMP));
  //RUNNING
  digitalWrite(PINC_RUNNING, TSTR_Linear_Variable_Update(TSTR_Test.time, RUNNING));

}



