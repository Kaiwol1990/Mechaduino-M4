
#include "commands/SerialCalibrate.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "core/Calibration.h"

#include "language/en.h"

void init_calibrate_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd(calibrate_command, "start calibration routine", cmd_calibration);
}

void cmd_calibration()
{
  calibration();
}
