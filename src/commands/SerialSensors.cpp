
#include "commands/SerialSensors.h"

#include "core/State.h"

#include "language/en.h"

void init_sensor_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd("temp", "returns the current coil Temepratur of the " ProductName, getTemperature);
  myCommander.cmdAdd("tempCPU", "returns the current CPU Temepratur of the " ProductName, getCPUTemperature);
  myCommander.cmdAdd("volt", "returns the current coil Voltage of the " ProductName, getVoltage);
}

void getVoltage()
{
  Serial.print("Current Voltage: ");
  Serial.println(mysamd51ADCSM.getV());
}

void getTemperature()
{
  Serial.print("Current Temperature: ");
  Serial.println(mysamd51ADCSM.getT());
}

void getCPUTemperature()
{
  Serial.print("Current Temperature: ");
  Serial.println(mysamd51ADCSM.getTCPU());
}