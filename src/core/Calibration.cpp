
#include "core/Calibration.h"
#include "core/State.h"
#include "core/Utils.h"
#include "core/settings.h"

#include "commands/Serial.h"

#include "SAMD51/interrupts.h"

#include "OUTPUT/A4954.h"
#include "Configuration.h"
#include "language/en.h"

#include "modules/custommath.h"
#include "modules/Cmd.h"

#include "Configuration.h"

#define effort 30.0
//const float stepangle = (360.0 / (Init_steps_per_revolution * Init_microstepping)); // angle of one microstep as float

void calibration()
{

  //float calibCurrent = 700.0;
  float currentAngle = 0.0;
  float currentAngle_1 = 0.0;

  mysamd51TC4.disable(); //disableTC5Interrupts();
  myPID.disable();

  int avg = 100;

  Serial.println(calibrate_header);
  Serial.println();

  //REG_PORT_OUTSET1 = PORT_PB16;  //write D3 HIGH to turn on V_Motor
  delay(200);

  //dir = true;
  //  enabled = true;
  //myPID.enable();
  steps = 0;
  //myA4954.outputOpenloop(0.0, M_max * 0.5);
  //myA4954.writeDACS(0, M_max * 0.5);
  myA4954.output(0, effort);
  delay(100);

  float encoderReading = 0;

  for (int i = 0; i < 50; i++)
  {
    encoderReading = encoderReading + myAS5047D.readDigits();
    delayMicroseconds(250);
  }
  encoderReading = encoderReading / 50.0;

  for (int i = 0; i < 4; i++)
  {
    currentAngle = currentAngle + (360.0 / Init_steps_per_revolution);
    for (int i = 0; i < 100; i++)
    {
      myA4954.outputOpenloop(currentAngle_1 + (currentAngle - currentAngle_1) * ((float)i / 100.0), effort);
      delayMicroseconds(250);
    }
    currentAngle_1 = currentAngle;
  }

  float temp_pos = 0;
  for (int i = 0; i < 50; i++)
  {
    temp_pos = temp_pos + myAS5047D.readDigits();
    delayMicroseconds(250);
  }
  temp_pos = temp_pos / 50.0;

  if ((temp_pos - encoderReading) < 0)
  {
    Serial.println("Wired backwards");
    myPID.disable();
    mysamd51TC4.enable(); //enableTC5Interrupts();
    return;
  }

  for (int i = 0; i < 4; i++)
  {
    currentAngle = currentAngle - (360.0 / Init_steps_per_revolution);
    for (int i = 0; i < 100; i++)
    {
      myA4954.outputOpenloop(currentAngle_1 + (currentAngle - currentAngle_1) * ((float)i / 100.0), effort);
      delayMicroseconds(250);
    }
    currentAngle_1 = currentAngle;
  }

  Serial.println("Calibrating fullsteps");
  Serial.println(procent_bar);
  int counter = 0;
  int count = Init_steps_per_revolution / 50;
  dir = true;

  int16_t readings[(int)Init_steps_per_revolution];

  int temp_reading = myAS5047D.readDigits();
  int last_reading = temp_reading;

  steps = 0;
  // step to every single fullstep position and read the Encoder
  for (int i = 0; i < Init_steps_per_revolution; i++)
  {

    counter += 1;

    delay(350);

    // flush the encoder
    myAS5047D.readDigits();
    myAS5047D.readDigits();

    // start real readings
    encoderReading = 0;
    temp_reading = myAS5047D.readDigits();
    last_reading = temp_reading;

    for (int k = 0; k < avg; k++)
    {

      temp_reading = myAS5047D.readDigits();

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

    readings[i] = (encoderReading / avg) + 0.5;
    if (readings[i] > 16384)
    {
      readings[i] = readings[i] - 16384;
    }

    if (counter == count)
    {
      counter = 0;
      Serial.print(".");
    }

    // increment angle one step
    currentAngle = currentAngle + (360.0 / Init_steps_per_revolution);
    for (int i = 0; i < 100; i++)
    {
      myA4954.outputOpenloop(currentAngle_1 + (currentAngle - currentAngle_1) * ((float)i / 100.0), effort);
      delayMicroseconds(250);
    }
    currentAngle_1 = currentAngle;
  }
  Serial.println(" -> done!");

  myA4954.outputOpenloop(0.0, 0.0);

  steps = 0;
  myPID.disable();

  /*

  // print full table to serial monitor
  Serial.println("");
  Serial.print("int16_t fullsteps[] ={");
  for (int i = 0; i < Init_steps_per_revolution; i++)
  {
    Serial.print(readings[i]);

    if (i < Init_steps_per_revolution - 1)
    {
      if ((mod(i, 25) == 0) & (i > 0))
      {
        Serial.println(",");
      }
      else
      {
        Serial.print(",");
      }
    }
  }
  Serial.println("};");
  Serial.println(" ");
  Serial.println(" ");
*/

  //mySettings.fullsteps.data = readings;
  mySettings.saveFullsteps(readings);

  myAS5047D.initTable(readings);

  myPID.disable();
  mysamd51TC4.enable(); //enableTC5Interrupts();
}
