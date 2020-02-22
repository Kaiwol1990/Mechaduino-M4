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
  AS5047D.readAngle();
  y = AS5047D.getAngle();

  // -------------------------------------
  // -         read step register        -
  // -------------------------------------
  steps = stepInterface.readsteps();

  // -------------------------------------
  // -           Motion planing          -
  // -------------------------------------
  r = Planner.setTarget(Settings.stepangle * (float)steps);

  // -------------------------------------
  // -       Reset if not enabled       -
  // -------------------------------------
  if (!PID.getState())
  {
    stepInterface.writeangle(y);
  }

  // -------------------------------------
  // -           PID loop           -
  // -------------------------------------
  PID.compute();

  // -------------------------------------
  // -         Current Controller        -
  // -------------------------------------
  if (Monitoring.errorOccured())
  {
    A4954.output(y, 0);
  }
  else
  {
    A4954.output(y, u);
  }
}

void StateMachine()
{
  // Motor Voltage and Coil Temp check
  mysamd51ADCSM.step();
}