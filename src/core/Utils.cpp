//Contains the definitions of the functions used by the firmware.
#include <SPI.h>
#include <Arduino.h>
#include "Configuration.h"
#include "SAMD51/board.h"
#include "SAMD51/interrupts.h"
#include "SAMD51/setup.h"

#include "core/Controler.h"
#include "core/State.h"
#include "core/Serial.h"
#include "core/Cmd.h"
#include "core/Calibration.h"
#include "core/Utils.h"

#include "language/en.h"


void boot() {

  Serial.begin(baudrate);

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
  myA4954.setTorque(I_rated, M_max, iMAX);
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

  if (USE_ENABLE_PIN) {
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
  myPID.setTunings(Kp, Ki, Kd, Kf, Kv, Kac);
  myPID.setLimit(M_max);
  myPID.setIntegrationalLimit(M_max * 0.8);
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








void enaInterrupt() {
  if (USE_ENABLE_PIN) {
    if (digitalRead(ena_pin)) { // check if ena_pin is HIGH
      myPID.disable();
    }
    else {
      //myPID.enable();
      myPID.disable();
    }
  }
}






int measure_noise() {

  disableTC5Interrupts();


  delay(100);
  int counter = 0;
  float points[500] = {0};

  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);


  while (counter < 500) {
    now = micros();
    if (now > last_time + dt) {
      last_time = now;

      points[counter] = y;
      counter++;
    }
  }

  float mean = 0;
  for (int i = 0; i < 500; i++) {
    mean = mean + points[i];
  }
  mean = mean / 500.0;

  float upper = 0;
  float lower = 0;
  int upcounter = 0;
  int lowcounter = 0;

  for (int i = 0; i < 500; i++) {
    if (points[i] > mean ) {
      upcounter++;
      upper = upper + points[i];
    }
    else if (points[i] < mean) {
      lowcounter++;
      lower = lower + points[i];
    }
  }
  upper = upper / upcounter;
  lower = lower / lowcounter;

  float highest = mean;
  float lowest = mean;

  for (int i = 0; i < 500; i++) {
    if (points[i] > highest) {
      highest = points[i];
    }
    else if (points[i] < lowest) {
      lowest = points[i];
    }
  }

  enableTC5Interrupts();
  return abs((abs(highest) - abs(lowest)));

}












