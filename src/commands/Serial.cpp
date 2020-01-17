

#include "core/State.h"
#include "core/Calibration.h"

#include "language/en.h"

#include "modules/Cmd.h"

#include "commands/Serial.h"
#include "commands/SerialSettings.h"
#include "commands/SerialMovement.h"
#include "commands/SerialPID.h"
#include "commands/SerialTools.h"

void init_menu()
{

  cmdAdd(help_command, Serial_menu);
  cmdAdd(calibrate_command, cmd_calibration);
  cmdAdd(state_command, state);
  cmdAdd(reset_command, SoftReset);
  cmdAdd(step_response_command, step_response);
  cmdAdd(interrupt_command, set_TC);
  cmdAdd("loop", get_max_frequency);
  cmdAdd("motion", set_motion);
  cmdAdd("gains", set_gains);
  cmdAdd("spline", splineMovement);
  cmdAdd("pos", reportPostition);
  cmdAdd("settings", setting_menu);
}

void Serial_menu(int arg_cnt, char **args)
{

  Serial.println(help_header);
  Serial.println(help_command " - " help_menu);
  Serial.println(calibrate_command " - " calibrate_menu);
  Serial.println(diagnostics_command " - " diagnostics_menu);
  Serial.println(state_command " - " state_menu);
  Serial.println(stream_command " - " stream_menu);
  Serial.println(parameter_command " - " parameter_menu);
  Serial.println(error_command " - " error_menu);
  Serial.println(dirac_command " - " dirac_menu);
  Serial.println(step_response_command " - " step_response_menu);
  Serial.println(error_command " - " error_menu);
  Serial.println(autotune_command " - " autotune_menu);
  Serial.println(downhill_command " - " downhill_menu);
  Serial.println(test_command " - " test_menu);
  Serial.println(interrupt_command " - " interrupt_menu);
}

void cmd_calibration(int arg_cnt, char **args)
{
  calibration();
}

void reportPostition(int arg_cnt, char **args)
{
  Serial.print("Target = ");
  Serial.print(r);
  Serial.print(", ");
  Serial.print("Postion = ");
  Serial.println(y);
}

void state(int arg_cnt, char **args)
{
  Serial.println(state_header);

  if (check_argument(args, arg_cnt, "-off"))
  {
    Serial.println("disabling");
    myPID.disable();
  }
  if (check_argument(args, arg_cnt, "-on"))
  {
    Serial.println("enabling");
    myPID.enable();
  }

  Serial.print(" Motor state = ");
  Serial.println(myPID.getState());
}
