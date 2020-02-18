
#include "commands/SerialMovement.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

void init_movement_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd("motion", "set speed and acceleration", set_motion);
  myCommander.cmdAdd(step_response_command, "execute a step response", step_response);
  myCommander.cmdAdd("spline", "start spline movement", splineMovement);
  myCommander.cmdAdd("pos", "print current position", reportPostition);
}

void reportPostition()
{
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Prints the current position and target on the console");
    Serial.println(" ");
    return;
  }
  Serial.print("Target = ");
  Serial.print(r);
  Serial.print(", ");
  Serial.print("Postion = ");
  Serial.println(y);
}

void splineMovement()
{
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Menu to execute spline movement with optional velocity and acceleration");
    Serial.println(" ");
    Serial.println(" spline -r [x] -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-r [x]  target angle in deg");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println(" ");
    return;
  }

  // check for given velocity
  velLimit = myCommander.return_float_argument("-v", velLimit, 100, 5000);
  accLimit = myCommander.return_float_argument("-a", accLimit, 100, 50000);

  // calculate Target
  steps = ((myCommander.return_float_argument("-r", r, -100000.0, 100000.0) * mySettings.currentSettings.steps_per_Revolution * mySettings.currentSettings.microstepping) / 360.0) + 0.5;
  mystepInterface.writesteps(steps);
}

void set_motion()
{
  //if (myCommander.check_argument(args, arg_cnt, help_subcmd))
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Menu to maximal velocity and acceleration");
    Serial.println(" ");
    Serial.println(" motion -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println(" ");
    return;
  }

  float velLimit_temp = myCommander.return_float_argument("-v", velLimit, 1, 1000000);
  float accLimit_temp = myCommander.return_float_argument("-acc", accLimit, 1, 1000000);
  velLimit = velLimit_temp;
  accLimit = accLimit_temp;
}

void step_response()
{
  //if (myCommander.check_argument(args, arg_cnt, help_subcmd))
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Menu execute a step response");
    Serial.println(" ");
    Serial.println(" response -r [x] -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-r [x]  motion length in deg");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println(" ");
    return;
  }

  Serial.println(step_response_header);

  int response_steps = ((myCommander.return_float_argument("-r", r, -1000.0, 1000.0) * mySettings.currentSettings.steps_per_Revolution * mySettings.currentSettings.microstepping) / 360.0) + 0.5;

  // check for given velocity
  velLimit = myCommander.return_float_argument("-v", velLimit, 100, 5000);
  accLimit = myCommander.return_float_argument("-a", accLimit, 100, 50000);

  int frequency = 5000;

  Serial.print("Steps = ");
  Serial.println(response_steps);
  Serial.print("Frequency = ");
  Serial.println(frequency);

  bool last_dir = dir;

  float answer[2000];
  float target[2000];
  float effort[2000];

  int counter = 0;

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  float dt = 1000000.0 / (float)frequency;

  // calculate the target vector
  target[0] = mystepInterface.readsteps();
  for (int i = 1; i < 2000; i++)
  {
    target[i] = target[i - 1];

    if (i == 200)
    {
      target[i] = target[i - 1] + response_steps;
    }
  }

  dir = true;
  //enabled = true;
  myPID.enable();
  mystepInterface.writesteps(target[0]);
  delay(2000);
  while (counter < 2000)
  {
    current_time = micros();

    if (current_time >= next_time)
    {
      next_time = current_time + dt;

      answer[counter] = myAS5047D.getAngle();
      effort[counter] = u;
      mystepInterface.writesteps(target[counter]);
      target[counter] = r;
      //steps = target[counter];

      counter += 1;
    }
  }

  Serial.println("time;target;anlge;effort");
  for (int i = 0; i < 2000; i++)
  {
    Serial.print((float)((float)i * (float)dt / 1000000.0), 5);
    Serial.print(';');
    Serial.print(target[i], 5);
    Serial.print(';');
    Serial.print(answer[i], 5);
    Serial.print(';');
    Serial.println(effort[i], 5);
  }

  // set parameters back to the values before the
  //enabled = last_enabled;
  myPID.disable();
  dir = last_dir;
}