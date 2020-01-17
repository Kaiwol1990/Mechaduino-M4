
#include "commands/SerialPID.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

void set_gains(int arg_cnt, char **args)
{
  // set corresponding gain in the current setting slot
  mySettings.currentSettings.Kp = return_float_argument(args, arg_cnt, "-Kp", mySettings.currentSettings.Kp, 0.0, 100.0);
  mySettings.currentSettings.Ki = return_float_argument(args, arg_cnt, "-Ki", mySettings.currentSettings.Ki, 0.0, 1000.0);
  mySettings.currentSettings.Kd = return_float_argument(args, arg_cnt, "-Kd", mySettings.currentSettings.Kd, 0.0, 100.0);
  mySettings.currentSettings.Kf = return_float_argument(args, arg_cnt, "-Kf", mySettings.currentSettings.Kf, 0.0, 100.0);
  mySettings.currentSettings.Kv = return_float_argument(args, arg_cnt, "-Kv", mySettings.currentSettings.Kv, 0.0, 100.0);
  mySettings.currentSettings.Kac = return_float_argument(args, arg_cnt, "-Kac", mySettings.currentSettings.Kac, 0.0, 100.0);

  // print report of the pid settings
  mySettings.reportPID();

  // upadte controller loop
  myPID.updateGains();
}