#include <stdint.h>
#include "TSKR.h"

//Task
#define TSKR_1ms_count    1
#define TSKR_5ms_count    5
#define TSKR_10ms_count   10
#define TSKR_20ms_count   20
#define TSKR_100ms_count  100
#define TSKR_200ms_count  200
#define TSKR_500ms_count  500
#define TSKR_1s_count     1000
#define TSKR_NUM_TASK     8

const int TSKR_Task_Count[TSKR_NUM_TASK] = {
  TSKR_1ms_count,    
  TSKR_5ms_count,    
  TSKR_10ms_count,  
  TSKR_20ms_count,  
  TSKR_100ms_count, 
  TSKR_200ms_count,  
  TSKR_500ms_count,
  TSKR_1s_count 
};

const uint8_t TSKR_Select[TSKR_NUM_TASK] = {
  1, //1ms
  1, //5ms
  0, //10ms
  0, //20ms
  1, //100ms
  0, //200ms
  0, //500ms
  0  //1s
};

static uint8_t TSKR_Active[TSKR_NUM_TASK];
static uint8_t TSKR_Error[TSKR_NUM_TASK];

void TSKR_Scheduler(uint32_t count){
  static uint32_t TSKR_millis_prev;

  if(count == TSKR_millis_prev){
    return;
  }
  TSKR_millis_prev = count;
    uint8_t output = 0;

  for(int i = 0; i < TSKR_NUM_TASK; i++){
    //Only do checking on task rates selected for use
    if(TSKR_Select[i]){
        if(!(count % TSKR_Task_Count[i])){
            //if previous task has been completed or not if it has not set an error
            if(TSKR_Active[i]){
                TSKR_Error[i] = 1;
            }
            TSKR_Active[i] = 1;
            //output |= (1ul << i);
        }
    }
  }
  return;
}

uint8_t TSKR_Task_Read(uint8_t task_time){
  return TSKR_Active[task_time];
}

void TSKR_Task_Clear(uint8_t task_time){
  TSKR_Active[task_time] = 0;
}



