//Contains the declaration of the state variables for the control loop

#include "../Configuration.h"

#include "core/State.h"
#include "SAMD51/board.h"

//---- interrupt vars ----
volatile byte mode = 0;

float r = 0;       //target angle
float y = 0;       //current angle
int32_t steps = 0; //target step

float velLimit = 900.0;
float accLimit = 10000.0;

float error = 0; //error angle

float u = 0; // control effort

float electric_angle = 0;

bool dir = true; // flag for dir setting
                 //bool enabled = true;         // flag for enabled setting

AS5047D myAS5047D(&Serial, PORTA, 15);

A4954 myA4954(18, 19, 21, 20, 2, 5);

PIDControler myPID(&y, &u, &r);

stepInterface mystepInterface(step_pin, dir_pin, ena_pin, Init_steps_per_revolution, Init_microstepping);

userSettings mySettings;