//Contains TC5 Controller definition
//The main control loop is executed by the TC2 timer interrupt:

#include "core/Controler.h"
#include "core/objects.h"
#include "core/variables.h"
#include "core/Planner.h"

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
  r = myPlanner.setTarget(mySettings.stepangle * (float)steps);

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
  if (Monitoring.errorOccured())
  {
    myA4954.output(y, 0);
  }
  else
  {
    myA4954.output(y, u);
  }
}

void StateMachine()
{
  // Motor Voltage and Coil Temp check
  mysamd51ADCSM.step();
}