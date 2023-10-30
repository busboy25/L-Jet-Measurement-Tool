#include "HMII.h"

//This calibraiton was validated on 10/6/23
#define HMII_DEBOUNCE_THRES 2;

volatile struct _HMII_User{
  uint8_t select : 1;
  uint8_t debounce : 6;
  uint8_t :2;
} HMII_User;

//function tested on 10/6/23
extern uint8_t HMII_TestRead(){
  uint8_t test = 0;
  test |= (digitalRead(PINC_SWITCH0) << 0);
  test |= (digitalRead(PINC_SWITCH1) << 1);
  test |= (digitalRead(PINC_SWITCH2) << 2);
  test |= (digitalRead(PINC_SWITCH3) << 3);
  return test;
}
//function tested on 10/6/23
void HMII_UserButton_ISR(){
  HMII_User.select = 1;
}
//function tested on 10/6/23
extern uint8_t HMII_UserButton(){

  if(HMII_User.debounce){
    HMII_User.debounce --;
    HMII_User.select = 0;
    return 0;
  }
  if(HMII_User.select){
    HMII_User.debounce = HMII_DEBOUNCE_THRES;
    HMII_User.select = 0;
    return 1;
  }
  return 0;
}
//function tested on 10/6/23
extern void HMII_init(){
  pinMode(PINC_SWITCH0,     INPUT_PULLDOWN);
  pinMode(PINC_SWITCH1,     INPUT_PULLDOWN);
  pinMode(PINC_SWITCH2,     INPUT_PULLDOWN);
  pinMode(PINC_SWITCH3,     INPUT_PULLDOWN);
  pinMode(PINC_USERBUTTON,  INPUT_PULLUP);
  pinMode(PINC_TX_LED,      OUTPUT);
  pinMode(PINC_RX_LED,      OUTPUT);
  pinMode(PINC_BUILTIN_LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PINC_USERBUTTON), HMII_UserButton_ISR, RISING);
}
//function tested on 10/6/23
extern void HMIO_LED_Enable(bool led1, bool led2, bool led3){
  digitalWrite(PINC_TX_LED, !led1);
  digitalWrite(PINC_RX_LED, !led2);
  digitalWrite(PINC_BUILTIN_LED, !led3);
}
