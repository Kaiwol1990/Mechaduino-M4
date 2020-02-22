
#include "commands/SerialPID.h"

#include "core/objects.h"
#include "modules/Cmd.h"

#include "language/en.h"

void init_pid_menu()
{
  // generates the commands and dependencies for this "submenu"
  Commander.cmdAdd("gains", "set pid gains", set_gains);
}

void set_gains()
{
  if (Commander.check_argument(help_subcmd))
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
  Settings.currentSettings.Kp = Commander.return_float_argument("-Kp", Settings.currentSettings.Kp, 0.0, 100.0);
  Settings.currentSettings.Kp = Commander.return_float_argument("-Kp", Settings.currentSettings.Kp, 0.0, 100.0);
  Settings.currentSettings.Ki = Commander.return_float_argument("-Ki", Settings.currentSettings.Ki, 0.0, 1000.0);
  Settings.currentSettings.Kd = Commander.return_float_argument("-Kd", Settings.currentSettings.Kd, 0.0, 100.0);
  Settings.currentSettings.Kf = Commander.return_float_argument("-Kf", Settings.currentSettings.Kf, 0.0, 100.0);
  Settings.currentSettings.Kv = Commander.return_float_argument("-Kv", Settings.currentSettings.Kv, 0.0, 100.0);
  Settings.currentSettings.Kac = Commander.return_float_argument("-Kac", Settings.currentSettings.Kac, 0.0, 100.0);

  // print report of the pid settings
  if (Commander.check_argument("-show"))
  {
    reportPID();
  }

  // upadte controller loop
  PID.updateGains();
}

void reportPID()
{
  Serial.println(" ");
  Serial.println("Current PID Settings: ");
  Serial.println("--------------------------");

  Serial.print("Kp        = ");
  Serial.println(Settings.currentSettings.Kp);

  Serial.print("Ki        = ");
  Serial.println(Settings.currentSettings.Ki);

  Serial.print("Kd        = ");
  Serial.println(Settings.currentSettings.Kd);

  Serial.print("Kf        = ");
  Serial.println(Settings.currentSettings.Kf);

  Serial.print("Kv        = ");
  Serial.println(Settings.currentSettings.Kv);

  Serial.print("Kac       = ");
  Serial.println(Settings.currentSettings.Kac);

  Serial.print("Mmax      = ");
  Serial.println(Settings.currentSettings.Mmax);

  Serial.print("Int Limit = ");
  Serial.println(Settings.currentSettings.IntegralLimit);
}