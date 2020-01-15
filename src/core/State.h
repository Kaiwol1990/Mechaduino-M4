//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__
//#include <stdint.h>
#include <arduino.h>

#include "output/A4954.h"
#include "sensor/AS5047D.h"
#include "pid/PIDControler.h"
#include "modules/filters.h"

#include "SAMD51/stepInterface.h"

//---- interrupt vars ----

extern  float r;        //target angle
extern  float y;        //current angle
extern  int32_t steps;  //target step

extern float velLimit;
extern float accLimit;

extern  float error;      //error angle

extern  float u;            // control effort

extern  float electric_angle;

extern  bool dir;         // flag for  dir setting
//extern  bool enabled;     // flag for  enabled setting



//---- lookup table for the encoder reading ----
extern int16_t fullsteps[200];




// Coil Current
extern Filter filter_y;
extern Filter omegaFilter;

extern AS5047D myAS5047D;
extern A4954 myA4954;

extern PIDControler myPID;

extern stepInterface mystepInterface;

#endif
