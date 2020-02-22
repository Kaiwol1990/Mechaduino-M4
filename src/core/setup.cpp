//Contains the definitions of the functions used by the firmware.
#include "../Configuration.h"

#include "SAMD51/board.h"
#include "SAMD51/setup.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"

#include "core/Controler.h"
#include "core/objects.h"
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
  uint32_t startMillis = millis();
  while (!Serial && ((millis() - startMillis) < 1000))
  {
  }
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(bootscreen_01);
  Serial.println(bootscreen_02);
  Serial.println(bootscreen_03);
  Serial.println(bootscreen_04);
  Serial.println(bootscreen_05);
  Serial.println(bootscreen_06);
  Serial.println(bootscreen_07);
  Serial.println(bootscreen_08);
  Serial.println(bootscreen_09);
  Serial.println(bootscreen_10);
  Serial.println(bootscreen_11);
  Serial.println(bootscreen_12);
  Serial.println(" ");
  Serial.println(" ");

  // Load User settings from flash
  Serial.print("setup user settings: ");
  if (!Settings.isValid())
  {
    // only needed for first execution after compiling
    Settings.loadDefaultSettings();
    Settings.setDefaultSlot(0);
    Settings.storeSettings(0);
    Settings.commit();
  }
  else
  {
    Settings.loadSettings(Settings.defaultSlot);
  }

  // fullstep table
  Serial.print("setup AS5047: ");
  AS5047D.init(Settings.fullsteps.data);

  if (!Settings.checkFullsteps())
  {
    Serial.println("Fullsteps need to be calibrated!");
  }
  else
  {
    Serial.println(" OK");
  }

  Serial.print("setup pins: ");
  setupPins();
  Serial.println("OK");

  Serial.print("setup A4954_class: ");
  A4954.init();
  A4954.setTorque(Settings.currentSettings.Irated, Settings.currentSettings.Mmax, Settings.currentSettings.iMax);
  A4954.output(0, 0);
  Serial.println(" OK");

  Serial.print("setup StepInterface: ");
  stepInterface.setup();
  Serial.println("OK");

  // motion planning
  Serial.print("setup motion planner: ");
  Planner.setMode(mode_init);
  Serial.println("OK");

  if (Settings.currentSettings.use_enable)
  {
    Serial.print("setup enable pin: ");
    enaInterrupt();
    Serial.println("OK");
  }
  else
  {
    //PID.enable();
  }

  Serial.print("setup ADC state machine: ");
  mysamd51ADCSM.init();
  Serial.println("OK");

  Serial.print("setup PID Controller: ");
  PID.setSampleFreq(FPID);
  PID.updateGains();
  Serial.println("OK");

  Serial.print("setup Interrupts: ");
  //mysamd51TC0.setup();
  mysamd51TC4.setup();
  mysamd51TC5.setup();

  // mysamd51TC0.enable();
  mysamd51TC4.enable();
  mysamd51TC5.enable();
  Serial.println("OK");

  Commander.init();
}
