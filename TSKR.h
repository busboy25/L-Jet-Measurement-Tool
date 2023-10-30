#ifndef TSKR_h
#define TSKR_h
#include <Arduino.h>

enum TSKR_Count{
  t_1ms = 0,
  t_5ms,
  t_10ms,
  t_20ms,
  t_100ms,
  t_200ms,
  t_500ms,
  t_1s,
};

void TSKR_Scheduler(uint32_t count);

uint8_t TSKR_Task_Read(uint8_t task_time);

void TSKR_Task_Clear(uint8_t task_time);

#endif