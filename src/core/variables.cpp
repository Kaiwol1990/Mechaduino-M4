//Contains the declaration of the state variables for the control loop

#include "core/variables.h"

/****************************************** Variables ******************************************/
volatile float r = 0;     //target angle
volatile float y = 0;     //current angle
volatile float u = 0;     // control effort
volatile bool dir = true; // flag for dir setting

volatile int32_t steps = 0; //target step

float velLimit = 1200.0;
float accLimit = 15000.0;
