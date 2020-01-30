

#include "SAMD51/board.h"
#include "SAMD51/samd51TC4.h"

#include "core/State.h"
#include "core/Utils.h"

void enaInterrupt()
{
  if (Init_USE_ENABLE_PIN)
  {
    if (digitalRead(ena_pin))
    { // check if ena_pin is HIGH
      myPID.disable();
    }
    else
    {
      //myPID.enable();
      myPID.disable();
    }
  }
}

int measure_noise()
{

  mysamd51TC4.disable(); //disableTC5Interrupts();

  delay(100);
  int counter = 0;
  float points[500] = {0};

  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);

  while (counter < 500)
  {
    now = micros();
    if (now > last_time + dt)
    {
      last_time = now;

      points[counter] = y;
      counter++;
    }
  }

  float mean = 0;
  for (int i = 0; i < 500; i++)
  {
    mean = mean + points[i];
  }
  mean = mean / 500.0;

  float upper = 0;
  float lower = 0;
  int upcounter = 0;
  int lowcounter = 0;

  for (int i = 0; i < 500; i++)
  {
    if (points[i] > mean)
    {
      upcounter++;
      upper = upper + points[i];
    }
    else if (points[i] < mean)
    {
      lowcounter++;
      lower = lower + points[i];
    }
  }
  upper = upper / upcounter;
  lower = lower / lowcounter;

  float highest = mean;
  float lowest = mean;

  for (int i = 0; i < 500; i++)
  {
    if (points[i] > highest)
    {
      highest = points[i];
    }
    else if (points[i] < lowest)
    {
      lowest = points[i];
    }
  }

  mysamd51TC4.enable(); // enableTC5Interrupts();
  return abs((abs(highest) - abs(lowest)));
}
