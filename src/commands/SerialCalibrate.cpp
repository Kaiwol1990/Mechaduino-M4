
#include "commands/SerialCalibrate.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "core/Calibration.h"

#include "language/en.h"

void init_calibrate_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd(calibrate_command, "start calibration routine", calibration);
  myCommander.cmdAdd("fullsteps", "prints the current fullsteps to the serial monitor", printfullsteps);
}

void printfullsteps()
{
  Serial.println("Current fullsteps of the calibrated motor");
  for (uint8_t i = 0; i < 200; i++)
  {
    Serial.print(mySettings.fullsteps.data[i]);
    Serial.print(", ");
  }

  Serial.println(" ");
  Serial.println(" ");
}