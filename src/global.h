
/*
 * global.h
 *
 */
#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "sdkconfig.h"

#define pulse 10  // lengt of trigger puls triac
#define ZerroCrossPin 35
#define AC_pin1 32       // Output to Opto Triac1
#define AC_pin2 25       // Output to Opto Triac2
#define AC_pin3 33       // Output to Opto Triac3
#define GPIO_OUTPUT_PIN_SEL ((1ULL << AC_pin1) | (1ULL << AC_pin2) | (1ULL << AC_pin3))
#define GPIO_INPUT_PIN_SEL (1ULL << ZerroCrossPin)


#endif /* GLOBAL_H_ */
