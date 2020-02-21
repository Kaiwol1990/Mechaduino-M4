//Contains the declaration of the state variables for the control loop

#ifndef __VARIABLES_H__
#define __VARIABLES_H__
//#include <stdint.h>

#include <arduino.h>

//---- interrupt vars ----

extern volatile float r;       //target angle
extern volatile float y;       //current angle
extern volatile int32_t steps; //target step
extern volatile float u;       // control effort
extern volatile bool dir;      // flag for  dir setting

extern float velLimit;
extern float accLimit;

#endif
