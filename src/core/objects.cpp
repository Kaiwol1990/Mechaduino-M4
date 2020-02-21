//Contains the declaration of the state variables for the control loop

#include "../Configuration.h"

#include "core/objects.h"
#include "core/variables.h"
#include "core/Controler.h"
#include "SAMD51/board.h"
#include "commands/Serial.h"

//---- interrupt vars ----
//volatile byte mode = 0;
/*
volatile float r = 0;     //target angle
volatile float y = 0;     //current angle
volatile float u = 0;     // control effort
volatile bool dir = true; // flag for dir setting

volatile int32_t steps = 0; //target step

float velLimit = 1200.0;
float accLimit = 15000.0;*/

AS5047D myAS5047D(&Serial, PORTA, 15);

A4954 myA4954(18, 19, 21, 20, 2, 5);

PIDControler myPID(&y, &u, &r);

stepInterface mystepInterface(step_pin, dir_pin, ena_pin, Init_steps_per_revolution, Init_microstepping);

userSettings mySettings;

SerialCommander myCommander(&Serial, init_menu);

samd51TC4 mysamd51TC4(FPID, ControlerLoop);

samd51TC5 mysamd51TC5(250, StateMachine);

samd51TC0 mysamd51TC0(25);

samd51ADCSM mysamd51ADCSM;

samd51TRNG mysamd51TRNG;