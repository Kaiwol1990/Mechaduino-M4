//Contains the definitions of the functions used by the firmware.
#include <SPI.h>
#include <Arduino.h>
#include "../Configuration.h"

#include "SAMD51/board.h"
#include "SAMD51/interrupts.h"
#include "SAMD51/setup.h"

#include "core/Controler.h"
#include "core/State.h"
#include "core/Serial.h"
#include "core/Cmd.h"
#include "core/Calibration.h"
#include "core/boot.h"
#include "core/Utils.h"

#include "language/en.h"



void boot() {

  Serial.begin(Init_baudrate);

  delay(3000);

  Serial.print("setup pins:");
  setupPins();
  Serial.println(" OK");

  Serial.print("setup AS5047:");
  // Calculating the lookuptable at startup.
  myAS5047D.init(fullsteps);
  myAS5047D.readDIAAGC();
  myAS5047D.readERRFL();
  //myAS5047D.setOffset(-98.85);
  //  Serial.print("setup lookup:");
  //  calculateLookup(fullsteps);
  //  Serial.println(" OK");
  Serial.println(" OK");


  Serial.print("setup A4954:");
  myA4954.init();
  myA4954.setTorque(Init_I_rated, Init_M_max, Init_iMAX);
  myA4954.output(0, 0);
  Serial.println(" OK");

  Serial.print("setup StepInterface:");
  mystepInterface.setup();
  Serial.println(" OK");

  Serial.print("setup SPI:");
  // setupSPI();
  Serial.println(" OK");

  Serial.print("setup Interrupts:");
  setupTCInterrupts();
  Serial.println(" OK");

  if (Init_USE_ENABLE_PIN) {
    Serial.print("setup enable pin:");
    enaInterrupt();
  }
  else {
    myPID.enable();
  }
  Serial.println(" OK");

  Serial.print("setup direction pin:");
  //dirInterrupt();
  Serial.println(" OK");


  Serial.print("setup PID Controller:");
  myPID.setSampleFreq(FPID);
  // FIXME
  // Load from eeprom instead of init hardcoded values if possible
  myPID.setTunings(Init_Kp, Init_Ki, Init_Kd, Init_Kf, Init_Kv, Init_Kac);
  myPID.setLimit(Init_M_max);
  myPID.setIntegrationalLimit(Init_M_max * 0.8);
  Serial.println(" OK");


  Serial.print("setup controller:");
  enableTC5Interrupts();
  Serial.println(" OK");




  Serial.print("generating filter variables:");
  filter_y.init();
  omegaFilter.init();
  Serial.println(" OK");

  cmdInit(&Serial);

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


  Serial.println("");
  Serial.print("CMD >> ");
  
  init_menu();
}





