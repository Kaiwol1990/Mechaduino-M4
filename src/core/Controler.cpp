//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include "Configuration.h"

#include "core/Controler.h"
#include "core/State.h"
#include "core/Utils.h"
#include "core/Planner.h"

#include "modules/custommath.h"

// ----- gets called with FPID -----
void ControlerLoop()
{

  // -------------------------------------
  // -      read the current angle       -
  // -------------------------------------
  myAS5047D.readAngle();
  y = myAS5047D.getAngle();

  // -------------------------------------
  // -         read step register        -
  // -------------------------------------
  steps = mystepInterface.readsteps();

  // -------------------------------------
  // -           Motion planing          -
  // -------------------------------------
  r = trapezodialSpline(mySettings.stepangle * (float)steps);

  // -------------------------------------
  // -       Reset if not enabled       -
  // -------------------------------------
  if (!myPID.getState())
  {
    mystepInterface.writeangle(y);
  }

  // -------------------------------------
  // -           PID loop           -
  // -------------------------------------
  myPID.compute();

  // -------------------------------------
  // -         Current Controller        -
  // -------------------------------------
  myA4954.output(y, u);
}

void StateMachine()
{
  // Motor Voltage and Coil Temp check
  mysamd51ADCSM.step();
}