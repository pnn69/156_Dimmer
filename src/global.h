
/*
 * global.h
 *
 */
#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "sdkconfig.h"

#define pulse 39  // lengt of trigger puls triac
#define ZerroCrossPin 21
#define AC_pin1 19       // Output to Opto Triac1
#define AC_pin2 22       // Output to Opto Triac2
#define AC_pin3 21       // Output to Opto Triac3
#define GPIO_OUTPUT_PIN_SEL ((1ULL << AC_pin1) | (1ULL << AC_pin2) | (1ULL << AC_pin3))
#define GPIO_INPUT_PIN_SEL (1ULL << ZerroCrossPin)


#endif /* GLOBAL_H_ */
