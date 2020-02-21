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
// r = myPlanner.target(mySettings.stepangle * (float)steps);
#ifdef Use_Spline
  r = splineInterpolate(mySettings.stepangle * (float)steps);
#else
  r = trapezodialSpline(mySettings.stepangle * (float)steps);
#endif

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