
#include "commands/SerialTools.h"

#include "core/objects.h"
#include "core/Utils.h"
#include "modules/Cmd.h"

#include "language/en.h"

#include "core/Controler.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"

void init_tools_menu()
{
  // generates the commands and dependencies for this "submenu"
  Commander.cmdAdd(reset_command, "reset the Mechaduino", SoftReset);
  Commander.cmdAdd("loop", "measure the maximal frequency", get_max_frequency);
  Commander.cmdAdd("noise", "measure the encoder noise", measureNoise);
  Commander.cmdAdd(state_command, "get current state", state);
  Commander.cmdAdd("diagnose", "get current state", diagnose);
}
void diagnose()
{

  mysamd51TC4.disable();
  mysamd51TC5.disable();

  delay(100);

  Serial.println("Reading Encoder Diagnostics");
  Serial.println("---------------------------");
  AS5047D.readDIAAGC();
  AS5047D.readERRFL();

  Serial.println("Reading Temperature");
  Serial.println("---------------------------");
  Serial.println(mysamd51ADCSM.getT());
  Serial.println(" ");

  Serial.println("Reading Voltage");
  Serial.println("---------------------------");
  Serial.println(mysamd51ADCSM.getV());

  Monitoring.printError();

  mysamd51TC4.enable();
  mysamd51TC5.enable();
}

void measureNoise()
{

  mysamd51TC4.disable();
  mysamd51TC5.disable();

  float temp = AS5047D.measureNoise();

  Serial.print("Encoder noise in deg: ");
  Serial.println(temp);

  mysamd51TC4.enable();
  mysamd51TC5.enable();
}

void state()
{
  Serial.println(state_header);
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Menu to set the Mechaduino to a specific state");
    Serial.println(" ");
    Serial.println(" state -on / -off / -show");
    Serial.println(" ");
    Serial.println("-on    enables the Mechaduino");
    Serial.println("-off   disables the Mechaduino");
    Serial.println("-show  report the current state of the Mechaduino");
    Serial.println(" ");
    return;
  }

  if (Commander.check_argument("-off"))
  {
    Serial.println("disabling");
    PID.disable();
  }

  if (Commander.check_argument("-on"))
  {
    Serial.println("enabling");
    PID.enable();
  }

  if (Commander.check_argument("-show"))
  {
    Serial.print(" Motor state = ");
    Serial.println(PID.getState());
  }
}

void SoftReset()
{
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Resets the Mechaduino. The Serialloop will be closed!");
    Serial.println(" ");
    return;
  }
  Serial.println(reset_header);
  NVIC_SystemReset(); // processor software reset
}

void get_max_frequency()
{
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Calculates the maximal possbile PID frequency of the Mechaduino");
    Serial.println(" ");
    return;
  }
  mysamd51TC4.disable(); //disableTC5Interrupts();

  uint32_t max_counter = 10000;
  float frequency[10];

  PID.enable();

  uint32_t starting_test = micros();

  for (uint8_t k = 0; k < 10; k++)
  {

    uint32_t starting = micros();

    for (uint32_t i = 0; i < max_counter; i++)
    {
      ControlerLoop();
    }

    frequency[k] = 1000000.0 / ((float)(micros() - starting) / (float)max_counter);

    Serial.print("loop : ");
    Serial.print(k);
    Serial.print(" / 10 frequency [Hz] = ");
    Serial.println(frequency[k]);
  }

  PID.disable();

  float mean = 0.0;
  for (uint8_t k = 0; k < 10; k++)
  {
    mean += frequency[k];
  }
  mean = mean / 10.0;

  Serial.print("Mean frequency [Hz] = ");
  Serial.println(mean);
  Serial.print("execution time [ns] = ");
  Serial.println(1000000.0 / mean);

  Serial.print("Overalltime [s] = ");
  Serial.println((float)(micros() - starting_test) / 1000000.0);
  mysamd51TC4.enable(); //enableTC5Interrupts();
}
