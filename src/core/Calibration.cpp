
#include "core/Calibration.h"
#include "core/objects.h"

#include "OUTPUT/A4954.h"
#include "language/en.h"
#include "modules/custommath.h"

#define effort Settings.currentSettings.Mmax * 0.75
//#define avg 100

void calibration()
{
  // variables
  float openloopTarget = 0.0;
  float encoderReading = 0;
  float encoderReading1 = 0;

  // step delay
  // -> one full rotation while calibrating in 10 seconds
  int stepDelay = 10000 / Settings.currentSettings.steps_per_Revolution;

  int avg = 100;
  int temp_reading;
  int last_reading;

  int16_t readings[(int)Settings.currentSettings.steps_per_Revolution];

  // print header to serial port
  Serial.println(calibrate_header);
  Serial.println();

  // check if motor voltage is high enough
  if (mysamd51ADCSM.getV() < 10.0)
  {
    Serial.println("The Voltage of the MEchaduino M4 is below 10 V!");
    Serial.println("Please make sure to plugin the power supply!");
    Serial.println("-> abbort");
    return;
  }

  // disable timers
  mysamd51TC4.disable();
  mysamd51TC5.disable();
  PID.disable();

  // slowly increase current to jump to first fullstep position
  Serial.println("Going to next fullstep position");
  for (int i = 0; i < 100; i++)
  {
    A4954.outputOpenloop(0, effort * (float)i / 99.0);
    delay(1);
  }

  // read current position
  encoderReading = 0;
  for (int i = 0; i < 50; i++)
  {
    encoderReading = encoderReading + AS5047D.readDigits();
    delayMicroseconds(250);
  }
  encoderReading = encoderReading / 50.0;

  // move four steps
  Serial.println("moving 4 full steps in positive direction");
  for (int i = 0; i < 4000; i++)
  {
    openloopTarget += (Settings.PA / 1000.0);
    A4954.outputOpenloop(openloopTarget, effort);
    delayMicroseconds(250);
  }

  // read position once again
  encoderReading1 = 0;
  for (int i = 0; i < 50; i++)
  {
    encoderReading1 = encoderReading1 + AS5047D.readDigits();
    delayMicroseconds(250);
  }
  encoderReading1 = encoderReading1 / 50.0;
  /*
  // move back 4 steps
  Serial.println("moving 4 full steps in negative direction");
  for (int i = 0; i < 4000; i++)
  {
    openloopTarget -= (Settings.PA / 1000.0);
    A4954.outputOpenloop(openloopTarget, effort);
    delayMicroseconds(250);
  }
*/
  // check if direction of the move was right
  Serial.println("checking for correct direction of move");
  if (encoderReading1 < encoderReading)
  {
    Serial.println("Wired backwards: inverting direction");
    Settings.currentSettings.invert = true;

    // read current position
    encoderReading = 0;
    for (int i = 0; i < 50; i++)
    {
      encoderReading = encoderReading + AS5047D.readDigits();
      delayMicroseconds(250);
    }
    encoderReading = encoderReading / 50.0;

    // move four steps
    Serial.println("moving 4 full steps in positive direction");
    for (int i = 0; i < 4000; i++)
    {
      openloopTarget += (Settings.PA / 1000.0);
      A4954.outputOpenloop(openloopTarget, effort);
      delayMicroseconds(250);
    }

    // read position once again
    encoderReading1 = 0;
    for (int i = 0; i < 50; i++)
    {
      encoderReading1 = encoderReading1 + AS5047D.readDigits();
      delayMicroseconds(250);
    }
    encoderReading1 = encoderReading1 / 50.0;

    if (encoderReading1 < encoderReading)
    {
      PID.disable();
      mysamd51TC4.enable();
      return;
    }
    else
    {
      Serial.println("inverting was succsesfull");
    }
  }

  Serial.println("Calibrating fullsteps");
  Serial.println();
  Serial.println(procent_bar);

  // step to every single fullstep position and read the Encoder
  for (int i = 0; i < Settings.currentSettings.steps_per_Revolution; i++)
  {
    // wait for motor to settle
    delay(stepDelay);

    // flush the encoder
    AS5047D.readDigits();
    AS5047D.readDigits();

    // init readings
    encoderReading = 0;
    temp_reading = AS5047D.readDigits();
    last_reading = temp_reading;

    for (int k = 0; k < avg; k++)
    {

      temp_reading = AS5047D.readDigits();

      if ((temp_reading - last_reading) <= -8192)
      {
        temp_reading += 16384;
      }
      else if ((temp_reading - last_reading) >= 8192)
      {
        temp_reading -= 16384;
      }

      encoderReading += temp_reading;
      last_reading = temp_reading;

      delayMicroseconds(250);
    }

    readings[i] = (encoderReading / (float)avg) + 0.5;
    if (readings[i] > 16384)
    {
      readings[i] = readings[i] - 16384;
    }

    // print progressbar to serial port
    if (mod(i + 1, (int)Settings.currentSettings.steps_per_Revolution / 50) == 0)
    {
      Serial.print(".");
      if (i + 1 == Settings.currentSettings.steps_per_Revolution)
      {
        Serial.println();
      }
    }

    // slowly increment angle one reach next step
    for (int i = 0; i < 1000; i++)
    {
      openloopTarget += (Settings.PA / 1000.0);
      A4954.outputOpenloop(openloopTarget, effort);
      delayMicroseconds(25);
    }
  }

  // disable output
  A4954.outputOpenloop(0.0, 0.0);

  // save calibration values to flash
  Settings.saveFullsteps(readings);
  Settings.storeSettings(Settings.defaultSlot);
  Settings.commit();

  // generate lookuptable
  AS5047D.initTable(readings);

  // enable timers
  mysamd51TC4.enable();
  mysamd51TC5.enable();

  Serial.println("Calibration finished");
}