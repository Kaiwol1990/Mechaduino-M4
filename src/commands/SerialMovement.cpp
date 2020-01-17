
#include "commands/SerialMovement.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

void splineMovement(int arg_cnt, char **args)
{
  // check for given velocity
  velLimit = return_float_argument(args, arg_cnt, "-v", velLimit, 100, 5000);
  accLimit = return_float_argument(args, arg_cnt, "-a", accLimit, 100, 50000);

  // calculate Target
  steps = ((return_float_argument(args, arg_cnt, "-r", r, -100000.0, 100000.0) * Init_steps_per_revolution * Init_microstepping) / 360.0) + 0.5;
  mystepInterface.writesteps(steps);
}

void set_motion(int arg_cnt, char **args)
{
  float velLimit_temp = return_float_argument(args, arg_cnt, "-v", velLimit, 1, 1000000);
  float accLimit_temp = return_float_argument(args, arg_cnt, "-acc", accLimit, 1, 1000000);
  velLimit = velLimit_temp;
  accLimit = accLimit_temp;
}

void step_response(int arg_cnt, char **args)
{
  Serial.println(step_response_header);

  int response_steps = return_float_argument(args, arg_cnt, "-s", 1000, 1, 10000);

  // check for given velocity
  velLimit = return_float_argument(args, arg_cnt, "-v", velLimit, 100, 5000);
  accLimit = return_float_argument(args, arg_cnt, "-a", accLimit, 100, 50000);

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
  target[0] = mystepInterface.getsteps();
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