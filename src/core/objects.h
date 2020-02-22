//Contains the declaration of the state variables for the control loop

#ifndef __OBEJCTS_H__
#define __OBEJCTS_H__
//#include <stdint.h>

//#include <arduino.h>

#include "core/settings.h"
#include "core/Planner.h"
#include "core/monitoring.h"
#include "output/A4954.h"
#include "sensor/AS5047D.h"
#include "pid/PIDControler.h"
#include "modules/filters.h"
#include "SAMD51/stepInterface.h"
#include "SAMD51/samd51TC0.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"
#include "SAMD51/samd51TRNG.h"
#include "SAMD51/samd51ADCSM.h"
#include "modules/Cmd.h"

extern AS5047D_class AS5047D;

extern A4954_class A4954;

extern PIDControler PID;

extern stepInterface_class stepInterface;

extern userSettings Settings;

extern SerialCommander Commander;

extern samd51TC4 mysamd51TC4;

extern samd51TC5 mysamd51TC5;

//extern samd51TC0 mysamd51TC0;

extern samd51ADCSM mysamd51ADCSM;

extern samd51TRNG mysamd51TRNG;

extern Planner_class Planner;

extern Monitoring_class Monitoring;
#endif
