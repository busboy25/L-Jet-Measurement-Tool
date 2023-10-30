#include "AFMR.h"
#include <Arduino.h>
#include <stdint.h>


//function tested on 10/6/23
static __inline__ void syncDAC() __attribute__((always_inline, unused));
static void syncDAC(){
  while (DAC->STATUS.bit.SYNCBUSY == 1);
};
//function tested on 10/6/23
extern void AFMR_DAC_Enable(bool state){
  DAC->CTRLA.bit.ENABLE = state;     // Enable DAC
  syncDAC();
}
//function tested on 10/6/23
extern void ARMR_init() {
  //startup.c for SAMD21 board sets up the following:
  //DFLL48M as source for GLK0
  //XOSC32K as source for GLK1
  //OCS8M   as source for GLK3
  //these need to be single write functions
  //assigns GCLK0 to DAC
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_DAC));
  while(GCLK->STATUS.bit.SYNCBUSY);
  PM->APBCMASK.reg |= PM_APBCMASK_DAC;
  /*done in wiring.c as part of the arduino base init()
  DAC->CTRLB.reg |=  DAC_CTRLB_REFSEL_AVCC
                  |  DAC_CTRLB_EOEN;
  */
}
//function tested on 10/6/23
extern void ARMR_AFM_Update(uint16_t count){
  DAC->DATA.reg = count & 0x3FF;  // Anding with 0x3FF limits values to 1023 cause 10bit DAC
  syncDAC();
}

