#ifndef PINC_h
#define PINC_h
#include <Arduino.h>

/*
Seeed Xiao
Pin - Port  - Per Used  - Use
00    PA2     DAC         AFM OUT
01    PA4     GPIO        DIP SWITCH 0
02    PA10    GPIO        DIP SWITCH 1
03    PA11    GPIO        DIP SWITCH 2
04    PA8     GPIO        DIP SWITCH 3
05    PA9     EXTINT[9]   USER BUTTON IN
06    PB8     GPIO        RUNNING OUT
07    PB9     TC4[1]      IGN OUT
08    PA7     TCC1[1]     TEMP OUT
09    PA5     TCC0[1]     INJ OUT
10    PA6     TCC1[0]     BATT OUT

POWER LED GREEN
USER  LED YELLOW
TX    LED BLUE
RX    LED BLUE
*/

//HMII
#define PINC_SWITCH0    4
#define PINC_SWITCH1    3
#define PINC_SWITCH2    2
#define PINC_SWITCH3    1
#define PINC_USERBUTTON 5

//HMIO
#define PINC_TX_LED       11
#define PINC_RX_LED       12
#define PINC_BUILTIN_LED  13

//Test Outputs
#define PINC_IGN      7
#define PINC_IJN      9
#define PINC_AFM      0
#define PINC_VBATT    10
#define PINC_TEMP     8
#define PINC_RUNNING  6

#endif