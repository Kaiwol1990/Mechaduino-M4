

#include "SAMD51/setup.h"
#include <SPI.h>
#include <Arduino.h>



void setupPins() {

  // configure pins as input
  //  pinMode(ena_pin, INPUT_PULLDOWN); //D2


  // attachInterrupt(ena_pin, enaInterrupt, CHANGE);


  REG_PORT_DIRSET0 = PORT_PA08; // LED 1
  REG_PORT_DIRSET0 = PORT_PA09; // LED 2
  REG_PORT_DIRSET1 = PORT_PB16; // D3

  REG_PORT_OUTCLR0 = PORT_PA08;  // write LED 1 LOW
  REG_PORT_OUTCLR0 = PORT_PA09;  // write LED 2 LOW

  REG_PORT_OUTCLR1 = PORT_PB16;  // write D3 LOW

}



void setupSPI() {
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE1);

  SPI.begin();    //AS5047D SPI uses mode=1 (CPOL=0, CPHA=1)
  delay(1000);
  SPI.beginTransaction(settingsA);
}

