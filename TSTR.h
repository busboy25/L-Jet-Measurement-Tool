#ifndef TSTR_h
#define TSTR_h
#include <Arduino.h>

enum TSTR_variable{
  Time,       //0
  IgnPeriod,  //1
  IgnDwell,   //2
  InjPeriod,  //3
  InjPW,      //4
  AFM,        //5
  TEMP,       //6
  BATT,       //7
  RUNNING     //8
};


//TSTR
extern void TSTR_Setup(uint8_t testProfile);
extern uint8_t TSTR_Time_Update();
extern void TSTR_All_Variable_Update();
extern unsigned long TSTR_Linear_Variable_Update(unsigned long t, unsigned long variable);
extern long TSTR_Time_READ();
//Debug Functions
extern void TSTR_Time_Set(unsigned long t);
extern void TSTR_Position_Set(unsigned long pos);

#endif