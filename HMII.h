#ifndef HMII_H
#define HMII_H
#include <Arduino.h>
#include "PINC.h"

extern uint8_t HMII_TestRead();

extern uint8_t HMII_UserButton();

extern void HMII_init();

extern void HMIO_LED_Enable(bool led1, bool led2, bool led3);

#endif