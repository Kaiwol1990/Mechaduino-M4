//Contains the definitions of the functions used by the firmware.
#include "../Configuration.h"

#include "SAMD51/board.h"
#include "SAMD51/setup.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"

#include "core/Controler.h"
#include "core/State.h"
#include "core/Calibration.h"
#include "core/setup.h"
#include "core/Utils.h"
#include "core/settings.h"

#include "modules/Cmd.h"

#include "commands/Serial.h"

#include "language/en.h"

void setup()
{
  Serial.begin(Init_baudrate);
  delay(500);
  Serial.println(" ");
  Serial.println(" ");

  // Load User settings from flash
  if (!mySettings.isValid())
  {
    // only needed for first execution after compiling
    mySettings.loadDefaultSettings();
    mySettings.setDefaultSlot(0);
    mySettings.storeSettings(0);
    mySettings.commit();
  }
  else
  {
    mySettings.loadSettings(mySettings.defaultSlot);
  }

  // fullstep table
  Serial.print("setup AS5047:");
  if (!mySettings.checkFullsteps())
  {
    Serial.println("Fullsteps need to be calibrated!");
  }
  myAS5047D.init(mySettings.fullsteps.data);
  //myAS5047D.readDIAAGC();
  //myAS5047D.readERRFL();
  Serial.println(" OK");

  Serial.print("setup pins:");
  setupPins();
  Serial.println(" OK");

  Serial.print("setup A4954:");
  myA4954.init();
  myA4954.setTorque(mySettings.currentSettings.Irated, mySettings.currentSettings.Mmax, mySettings.currentSettings.iMax);
  myA4954.output(0, 0);
  Serial.println(" OK");

  Serial.print("setup StepInterface:");
  mystepInterface.setup();
  Serial.println(" OK");

  if (mySettings.currentSettings.use_enable)
  {
    Serial.print("setup enable pin:");
    enaInterrupt();
    Serial.println(" OK");
  }
  else
  {
    //myPID.enable();
  }

  Serial.print("setup ADC state machine");
  mysamd51ADCSM.init();
  Serial.println(" OK");

  Serial.print("setup PID Controller:");
  myPID.setSampleFreq(FPID);
  myPID.updateGains();
  Serial.println(" OK");

  Serial.print("setup Interrupts:");
  mysamd51TC4.setup();
  mysamd51TC4.enable();

  mysamd51TC5.setup();
  mysamd51TC5.enable();
  Serial.println(" OK");

  Serial.println(bootscreen_1);
  Serial.println(bootscreen_2);
  Serial.println(bootscreen_3);
  Serial.println(bootscreen_4);
  Serial.println(bootscreen_5);
  Serial.println(bootscreen_6);
  Serial.println(bootscreen_7);
  Serial.println(bootscreen_8);
  Serial.println(bootscreen_9);
  Serial.println(bootscreen_10);

  Serial.print("   compiling date: ");
  Serial.println(__DATE__);

  Serial.print("   firmware-version: ");
  Serial.println(firmware_version);

  myCommander.init();
}
