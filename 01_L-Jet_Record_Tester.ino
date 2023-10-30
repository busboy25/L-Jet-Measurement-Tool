/* 
 *  Task Scheduler v0.1
 *  Ben Dean
 *  Updated 9/8/2023 
 *    (Design, Code, Notes, and Validation)
 *  
 *  Features:
 *  
 *  Overview:
 *   
 *  
 *  Validation Notes:
 *    Version 0.1 untested on 10/18/2020 
 *  
 *  Version Control:
 *    Version 1 = 
 */

#include <MemoryFree.h>

#include "AFMR.h"
#include "HMII.h"
#include "PINC.h"
#include "STMR.h"
#include "TIMR.h"
#include "TSKR.h"
#include "TSTR.h"


#define debug0    0
#define debug1    0
#define debug_output 0
#define debug_HMII 0
#define debug_serial 1
#define debug_state 0
#define debug_serial_100 0
#define debug_serial_1 0
#define debug_serial_5 0
#define debug_test 0

void setup(){
  while(debug0){

  }

  if(debug_serial){
    Serial.begin(115200);
    while(!Serial){
      ;// wait for serial port to connect. Needed for native USB
    }
    Serial.print(F("Serial Complete. Starting Free RAM ="));
    Serial.println(freeMemory());
  }


  TIMR_init(); //Setup Timer for Ign, Inj, and PWM DAC
  ARMR_init(); //Setup DAC for AFM
  HMII_init(); //Setup HMI Port Configs

  if(debug_serial){
    Serial.println(F("Setup Complete"));
  }
}

void loop(){
  // put your main code here, to run repeatedly:
  if(debug_output){
    
    ARMR_AFM_Update(500);
    TIMR_BATT_CC_Update(200);
    TIMR_TEMP_CC_Update(700);
    TIMR_INJ_Timer_Update(8000, 5000);
    TIMR_IGN_Timer_Update(30000, 20000);

    //AFMR_DAC_Enable(1);
    //TIMR_TCC0_Enable(1);
    //TIMR_TCC1_Enable(1);
    //TIMR_TC4_Enable(1);
    if(debug_serial){
      Serial.println("Output Load");
    }
  }

  while(debug_HMII){
    bool i = 0;
    bool j = 0;
    bool g = 0;
    HMIO_LED_Enable(i, j, g);
    while(1){
      delay(100); //delay minics 100ms task
      if(HMII_UserButton()){
        g = j;
        j = i;
        i = !i;
        int value = HMII_TestRead();
        if(debug_serial){
          Serial.println(value, HEX);
        }
        HMIO_LED_Enable(i, j, g);
      }
    }
  }

  while(debug_state){
    int value = STMR_StateMachine();
    //Serial.println(value);
    delay(100);
  }

  while(debug_test){
   
    TSTR_Setup(0b0011);
    for(int f = 0; f < 3001; f++){
      TSTR_Time_Update();
      if(!(f%5)){
        TSTR_All_Variable_Update();
      }
    }

    while(1){

    }
  }

  static int state; 

  TSKR_Scheduler(millis());
  
  if(TSKR_Task_Read(t_100ms)){
    if(debug_serial_100){
      Serial.print(F("100ms: Millis ="));
      Serial.println(millis());
      Serial.print(F("Free RAM ="));
      Serial.println(freeMemory());
    }
    state = STMR_StateMachine();
    TSKR_Task_Clear(t_100ms);
  }

  if(TSKR_Task_Read(t_1ms) && (state == NonContinuousRunning)){
    if(debug_serial_1){
      Serial.println(F("1ms Step 1"));
    }
    int checkState = TSTR_Time_Update();
    STMR_State_Update(checkState);

    if(checkState == Ending){
      state = STMR_StateMachine();
    }

    if(debug_serial_1){
      Serial.println(F("1ms Step 2"));
    }

    TSKR_Task_Clear(t_1ms);
  }

  if(TSKR_Task_Read(t_5ms) && (state == NonContinuousRunning)){
    if(debug_serial_5){
      Serial.println("5ms");
      Serial.print(F("Free RAM ="));
      Serial.println(freeMemory());
    }    
    TSTR_All_Variable_Update();
    
    TSKR_Task_Clear(t_5ms);
  }

}


