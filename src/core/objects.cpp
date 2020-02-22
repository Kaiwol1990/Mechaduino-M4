//Contains the declaration of the state variables for the control loop

#include "../Configuration.h"

#include "core/objects.h"
#include "core/variables.h"
#include "core/Controler.h"
#include "core/Planner.h"
#include "SAMD51/board.h"
#include "commands/Serial.h"

AS5047D_class AS5047D(&Serial, PORTA, 15);

A4954_class A4954(18, 19, 21, 20, 2, 5);

PIDControler PID(&y, &u, &r);

stepInterface_class stepInterface(step_pin, dir_pin, ena_pin, Init_steps_per_revolution, Init_microstepping);

userSettings Settings;

SerialCommander Commander(&Serial, init_menu);

samd51TC4 mysamd51TC4(FPID, ControlerLoop);

samd51TC5 mysamd51TC5(250, StateMachine);

//samd51TC0 mysamd51TC0(25);

samd51ADCSM mysamd51ADCSM;

samd51TRNG mysamd51TRNG;

Planner_class Planner;

Monitoring_class Monitoring;