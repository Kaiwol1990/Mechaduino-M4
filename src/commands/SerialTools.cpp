
#include "commands/SerialTools.h"

#include "core/State.h"
#include "core/Utils.h"
#include "modules/Cmd.h"

#include "language/en.h"

#include "core/Controler.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"

void init_tools_menu()
{
  // generates the commands and dependencies for this "submenu"
  myCommander.cmdAdd(reset_command, "reset the Mechaduino", SoftReset);
  myCommander.cmdAdd("loop", "measure the maximal frequency", get_max_frequency);
  myCommander.cmdAdd("noise", "measure the encoder noise", measure_noise);
  myCommander.cmdAdd(state_command, "get current state", state);
}

void state()
{
  Serial.println(state_header);
  if (myCommander.check_argument(help_subcmd))
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

  if (myCommander.check_argument("-off"))
  {
    Serial.println("disabling");
    myPID.disable();
  }

  if (myCommander.check_argument("-on"))
  {
    Serial.println("enabling");
    myPID.enable();
  }

  if (myCommander.check_argument("-show"))
  {
    Serial.print(" Motor state = ");
    Serial.println(myPID.getState());
  }
}

void SoftReset()
{
  if (myCommander.check_argument(help_subcmd))
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
  if (myCommander.check_argument(help_subcmd))
  {
    Serial.println("Calculates the maximal possbile PID frequency of the Mechaduino");
    Serial.println(" ");
    return;
  }
  mysamd51TC4.disable(); //disableTC5Interrupts();

  uint32_t max_counter = 10000;
  float frequency[10];

  myPID.enable();

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

  myPID.disable();

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
