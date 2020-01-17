
#include "commands/SerialPID.h"

#include "core/State.h"
#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

void init_pid_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd("gains", "set pid gains", set_gains);
}

void set_gains()
{
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Menu to modify the pid parameter of the motor");
    Serial.println(" ");
    Serial.println(" gains -Kp [x] -Ki [x] -Kd [x] -Kf [x] -Kv [x] -Kac [x]");
    Serial.println(" ");
    Serial.println("-Kp [x]   set the proportional gain");
    Serial.println("-Ki [x]   set the integrational gain");
    Serial.println("-Kd [x]   set the derivative gain");
    Serial.println("-Kf [x]   set the friction compensation gain");
    Serial.println("-Kv [x]   set the proportional velocity gain");
    Serial.println("-Kac [x]  set the proportional acceleration gain");
    Serial.println(" ");
    Serial.println("-show     shows the current active gains");
    Serial.println(" ");
    return;
  }
  // set corresponding gain in the current setting slot
  mySettings.currentSettings.Kp = myCommander.return_float_argument("-Kp", mySettings.currentSettings.Kp, 0.0, 100.0);
  mySettings.currentSettings.Kp = myCommander.return_float_argument("-Kp", mySettings.currentSettings.Kp, 0.0, 100.0);
  mySettings.currentSettings.Ki = myCommander.return_float_argument("-Ki", mySettings.currentSettings.Ki, 0.0, 1000.0);
  mySettings.currentSettings.Kd = myCommander.return_float_argument("-Kd", mySettings.currentSettings.Kd, 0.0, 100.0);
  mySettings.currentSettings.Kf = myCommander.return_float_argument("-Kf", mySettings.currentSettings.Kf, 0.0, 100.0);
  mySettings.currentSettings.Kv = myCommander.return_float_argument("-Kv", mySettings.currentSettings.Kv, 0.0, 100.0);
  mySettings.currentSettings.Kac = myCommander.return_float_argument("-Kac", mySettings.currentSettings.Kac, 0.0, 100.0);

  // print report of the pid settings
  if (myCommander.check_argument("-show"))
  {
    reportPID();
  }

  // upadte controller loop
  myPID.updateGains();
}

void reportPID()
{
  Serial.println(" ");
  Serial.println("Current PID Settings: ");
  Serial.println("--------------------------");

  Serial.print("Kp        = ");
  Serial.println(mySettings.currentSettings.Kp);

  Serial.print("Ki        = ");
  Serial.println(mySettings.currentSettings.Ki);

  Serial.print("Kd        = ");
  Serial.println(mySettings.currentSettings.Kd);

  Serial.print("Kf        = ");
  Serial.println(mySettings.currentSettings.Kf);

  Serial.print("Kv        = ");
  Serial.println(mySettings.currentSettings.Kv);

  Serial.print("Kac       = ");
  Serial.println(mySettings.currentSettings.Kac);

  Serial.print("Mmax      = ");
  Serial.println(mySettings.currentSettings.Mmax);

  Serial.print("Int Limit = ");
  Serial.println(mySettings.currentSettings.IntegralLimit);
}