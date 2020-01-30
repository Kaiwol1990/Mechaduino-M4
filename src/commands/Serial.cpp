

#include "core/State.h"

#include "language/en.h"

#include "modules/Cmd.h"

#include "commands/Serial.h"
#include "commands/SerialSettings.h"
#include "commands/SerialMovement.h"
#include "commands/SerialPID.h"
#include "commands/SerialTools.h"
#include "commands/SerialCalibrate.h"
#include "commands/SerialSensors.h"

void init_menu()
{
  myCommander.cmdAdd(help_cmd, "display all available commands", Serial_menu);

  init_settings_menu();
  init_movement_menu();
  init_pid_menu();
  init_tools_menu();
  init_calibrate_menu();
  init_sensor_menu();
}

//void Serial_menu()
void Serial_menu()
{
  Serial.println(" ");
  Serial.println("Mechaduino v2.0");
  Serial.println("---------------");
  Serial.println("Add -help to a command to get a detailed explenation!");
  Serial.println(" ");
  myCommander.printlist();
}
