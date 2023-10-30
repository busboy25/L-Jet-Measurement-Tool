#include "TIMR.h"
#include "TSTR.h"
#include <stdint.h>

static uint16_t TIMR_IGN_Value_Buffer[2];

//Syncs tested on 10/2/23
// Wait for synchronization of registers between the clock domains
static __inline__ void syncTC4() __attribute__((always_inline, unused));
static void syncTC4(){
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);
}

static __inline__ void syncTCC() __attribute__((always_inline, unused));
static void syncTCC0(){
  while (TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
}

static __inline__ void syncTCC() __attribute__((always_inline, unused));
static void syncTCC1(){
  while (TCC1->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
}

uint16_t limit(uint16_t value, uint16_t max, uint16_t min){
  if(value > max){
    return max;
  }else if(value < min){
    return min;
  }else{
    return value;
  }
}

extern void TIMR_TCC0_Enable(bool state){
  TCC0->CTRLA.bit.ENABLE = state;
  syncTCC0();
}

extern void TIMR_TCC1_Enable(bool state){
  TCC1->CTRLA.bit.ENABLE = state;
  syncTCC1();
}

extern void TIMR_TC4_Enable(bool state){
  TC4->COUNT16.CTRLA.bit.ENABLE = state;
  syncTC4();
}

inline void GCLK_Config(){
  //startup.c for SAMD21 board sets up the following:
  //DFLL48M as source for GLK0
  //XOSC32K as source for GLK1
  //OCS8M   as source for GLK3
  //these need to be single write functions
  //setup GLK4 to take OSCM8
  GCLK->GENCTRL.reg = (uint32_t)(GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(4));
  while(GCLK->STATUS.bit.SYNCBUSY);
  //set GCLK4 to have division factor of 10 
  GCLK->GENDIV.reg = (uint32_t)(GCLK_GENDIV_DIV(10) | GCLK_GENDIV_ID(4));
  while(GCLK->STATUS.bit.SYNCBUSY);
  //enable the GCLK4, assign the GCLK4 to TC4 and TC5
  //10/1/23 changed the uint16_t to a unit32_t
  GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while(GCLK->STATUS.bit.SYNCBUSY);
    //enable the GCLK4, assign the GCLK4 to TCC0 and TCC1
  GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
  while(GCLK->STATUS.bit.SYNCBUSY);
  //GCLK4 with division factor of 10 = 800kHz = 8MHz / 10 
}

void Port_Config(){
  //Port I/O Configuration for 
    //VBatt on Port PA6 multiplexed to TCC1[0] for peripheral E
  //PORT->Group[PORTA].DIRSET.reg     |= PORT_DIRSET_DIRSET(10) ; //Configure as output
  PORT->Group[PORTA].PMUX[6/2].reg |= PORT_PMUX_PMUXE_E;  //Configure peripheral E (TC/TCC) 5=10/2
  PORT->Group[PORTA].PINCFG[6].reg |= PORT_PINCFG_PMUXEN; //Perpheral is in control
    //Temp on Port PA7 multiplexed to TCC1[1] for peripheral E
  //PORT->Group[PORTA].DIRSET.reg     |= PORT_DIRSET_DIRSET(11) ; //Configure as output
  PORT->Group[PORTA].PMUX[7/2].reg |= PORT_PMUX_PMUXO_E;  //Configure to peripheral E (TC/TCC) 5=11/2
  PORT->Group[PORTA].PINCFG[7].reg |= PORT_PINCFG_PMUXEN; //Perpheral is in control
    //INJ on Port PA8 multiplexed to TCC0[0] for peripheral E
  //PORT->Group[PORTA].DIRSET.reg     |= PORT_DIRSET_DIRSET(8) ; //Configure as output
  PORT->Group[PORTA].PMUX[5/2].reg  |= PORT_PMUX_PMUXO_E;  //Configure peripheral E (TC/TCC) 4=8/2
  PORT->Group[PORTA].PINCFG[5].reg  |= PORT_PINCFG_PMUXEN; //Perpheral is in control
    //IGN Port PB8 multiplexed to TC4[0] for peripheral E
  //PORT->Group[PORTB].DIRSET.reg     |= PORT_DIRSET_DIRSET(8) ; //Configure PB08 as output
  PORT->Group[PORTB].PMUX[9/2].reg  |= PORT_PMUX_PMUXO_E;  //Configure PB08 to peripheral E (TC/TCC) 4=8/2
  PORT->Group[PORTB].PINCFG[9].reg |= PORT_PINCFG_PMUXEN; //Perpheral is in control of PB8
}

void TCC0_Config(){
  //Configure for PWM out for Injection signal
  
  //Disable TCC0
  TIMR_TCC0_Enable(0); 
  //for TCC0 use a divider of 8 to get 100kHz count clock
  TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV8;
  syncTCC0();
  //set TCC0 to NPWM  
  TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
  syncTCC0();
}

void TCC1_Config(){
  //Configure for PWM out for LPF DAC to create BATT and TEMP signals
  
  //Disable TCC1
  TIMR_TCC1_Enable(0); 
  //counter clock is defined in GCLKConfigure()
  //set prescaler
  //for TCC1 use a divider of 1 to get 800kHz count clock
  TCC1->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;
  syncTCC1();
  //set TCC1 to NPWM  
  TCC1->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
  syncTCC1();
    //set PER to 1024 which at a 800kHz clock is 0.00128 sec period or 781.25Hz
  TCC1->PER.reg = TCC1_PER;
  syncTCC1();
}

void TC4_Config(){
  //Configure for PWM outfor for Ignition signal
  
  TIMR_TC4_Enable(0);
  //counter clock is defined in GCLKConfigure()
  //set tc4 to 16bits, prescaler 8, setups match PWM outputs   
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16
                         | TC_CTRLA_PRESCALER_DIV8
                         | TC_CTRLA_WAVEGEN_MPWM; 
  syncTC4();

  //This allows the output be inverted
  //TC4->COUNT16.CTRLC.reg |= TC_CTRLC_INVEN0;
  //syncTC4();

  //enable interrupts for MC0 cause that is when the period is MATCH'd
  TC4->COUNT16.INTENSET.reg |= TC_INTENSET_MC0;
  NVIC_EnableIRQ(TC4_IRQn);

}

void TC4_Handler(){
  TC4->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;
  TC4->COUNT16.CC[0].reg = TIMR_IGN_Value_Buffer[0];
  syncTC4();
  TC4->COUNT16.CC[1].reg = TIMR_IGN_Value_Buffer[1];
  syncTC4();
  //Serial.println("TC4");
}

extern void TIMR_init(){
  GCLK_Config();
  //PM Configuration for TC4, TCC1, TCC0
  PM->APBCMASK.reg |= PM_APBCMASK_TC4
                    | PM_APBCMASK_TCC0
                    | PM_APBCMASK_TCC1;
  Port_Config();

  TCC0_Config();
  TCC1_Config();
  TC4_Config();
}

extern void TIMR_BATT_CC_Update(uint16_t count){
  TCC1->CC[0].reg = limit(count, BATT_PW_MAX, BATT_PW_MIN);
  syncTCC1();
}

extern void TIMR_BATT_CCB_Update(uint16_t count){
  TCC1->CCB[0].reg = limit(count, BATT_PW_MAX, BATT_PW_MIN);
  syncTCC1();
}

extern void TIMR_TEMP_CC_Update(uint16_t count){
  TCC1->CC[1].reg = limit(count, TEMP_PW_MAX, TEMP_PW_MIN);
  syncTCC1();
}

extern void TIMR_TEMP_CCB_Update(uint16_t count){
  TCC1->CCB[1].reg = limit(count, TEMP_PW_MAX, TEMP_PW_MIN);
  syncTCC1();
}

extern void TIMR_INJ_Timer_Update(uint16_t period, uint16_t pulse_width){
  TCC0->PER.reg = limit(period, INJ_PER_MAX, INJ_PER_MIN);
  syncTCC0();  
  TCC0->CC[1].reg = limit(pulse_width, period, INJ_PW_MIN);
  syncTCC0();
}

extern void TIMR_INJ_Buffer_Update(uint16_t period, uint16_t pulse_width){
  TCC0->PERB.reg = limit(period, INJ_PER_MAX, INJ_PER_MIN);
  syncTCC0();  
  TCC0->CCB[1].reg = limit(pulse_width, period, INJ_PW_MIN);
  syncTCC0();
}

extern void TIMR_IGN_Buffer_Update(uint16_t period, uint16_t pulse_width){
  TIMR_IGN_Value_Buffer[0] = period;
  TIMR_IGN_Value_Buffer[1] = pulse_width;
}

extern void TIMR_IGN_Timer_Update(uint16_t period, uint16_t pulse_width){
  TC4->COUNT16.CC[0].reg = limit(period, IGN_PER_MAX, IGN_PER_MIN);
  syncTC4();
  TC4->COUNT16.CC[1].reg = limit(pulse_width, period, IGN_PW_MIN);
  syncTC4();
}
