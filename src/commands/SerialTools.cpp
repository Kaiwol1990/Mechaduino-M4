
#include "commands/SerialPID.h"

#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

#include "core/Controler.h"
#include "SAMD51/interrupts.h"

void SoftReset(int arg_cnt, char **args)
{
  Serial.println(reset_header);
  NVIC_SystemReset(); // processor software reset
}

void set_TC(int arg_cnt, char **args)
{
  Serial.println(interrupt_header);

  bool TC5_bool = return_bool_argument(args, arg_cnt, "-TC5", true);

  if (TC5_bool)
  {
    Serial.println("TC5 enabled!");
    enableTC5Interrupts();
  }
  else
  {
    Serial.println("TC5 disabled!");
    disableTC5Interrupts();
  }
}

void get_max_frequency(int arg_cnt, char **args)
{
  disableTC5Interrupts();

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
  Serial.print("execution time = ");
  Serial.println(1000000.0 / mean);

  Serial.print("Overalltime [s] = ");
  Serial.println((float)(micros() - starting_test) / 1000000.0);
  enableTC5Interrupts();
}
