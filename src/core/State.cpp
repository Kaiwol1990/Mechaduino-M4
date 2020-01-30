//Contains the declaration of the state variables for the control loop

#include "../Configuration.h"

#include "core/State.h"
#include "core/Controler.h"
#include "SAMD51/board.h"
#include "commands/Serial.h"

//---- interrupt vars ----
volatile byte mode = 0;

volatile float r = 0;       //target angle
volatile float y = 0;       //current angle
volatile int32_t steps = 0; //target step

float velLimit = 900.0;
float accLimit = 10000.0;

//float error = 0; //error angle

volatile float u = 0; // control effort

float electric_angle = 0;

volatile bool dir = true; // flag for dir setting
                          //bool enabled = true;         // flag for enabled setting

AS5047D myAS5047D(&Serial, PORTA, 15);

A4954 myA4954(18, 19, 21, 20, 2, 5);

PIDControler myPID(&y, &u, &r);

stepInterface mystepInterface(step_pin, dir_pin, ena_pin, Init_steps_per_revolution, Init_microstepping);

userSettings mySettings;

SerialCommander myCommander(&Serial, init_menu);

samd51TC4 mysamd51TC4(FPID, ControlerLoop);

samd51TC5 mysamd51TC5(250, StateMachine);

samd51ADCSM mysamd51ADCSM;