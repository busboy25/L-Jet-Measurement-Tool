#ifndef TIMR_h
#define TIMR_h
#include <Arduino.h>
//#include "PINC.h"

#define TCC1_PER      1024
#define BATT_PW_MAX   1024 
#define TEMP_PW_MAX   1024
#define INJ_PER_MAX   60750
#define IGN_PER_MAX   30375

#define BATT_PW_MIN   0
#define TEMP_PW_MIN   0
#define INJ_PW_MIN    0
#define INJ_PER_MIN   750
#define IGN_PW_MIN    0
#define IGN_PER_MIN   375

//TIMR
extern void TIMR_init();
extern void TIMR_TCC0_Enable(bool state);
extern void TIMR_TCC1_Enable(bool state);
extern void TIMR_TC4_Enable(bool state);

//BATT FUNCTION
extern void TIMR_BATT_CC_Update(uint16_t batt_count);
extern void TIMR_BATT_CCB_Update(uint16_t count);

//TEMP FUNCTION
extern void TIMR_TEMP_CC_Update(uint16_t temp_count);
extern void TIMR_TEMP_CCB_Update(uint16_t count);

//INJ FUNCTIONS
extern void TIMR_INJ_Timer_Update(uint16_t period, uint16_t pulse_width);
extern void TIMR_INJ_Buffer_Update(uint16_t period, uint16_t pulse_width);

//IGN FUCNTION
extern void TIMR_IGN_Timer_Update(uint16_t period, uint16_t pulse_width);
extern void TIMR_IGN_Buffer_Update(uint16_t period, uint16_t pulse_width);


#endif