

#include <Arduino.h>

#ifndef _AS5047D_H_
#define _AS5047D_H_

class AS5047D
{
public:
  // public functions
  AS5047D(Stream *, EPortType port_, uint32_t pin_);

  // setup function
  void init(int16_t *fullsteps);

  // read angle of magnet in digits
  uint16_t readDigits();

  // read position of magnet and turn it into degrees with the calibrated lookup table
  void readAngle();
  __attribute__((always_inline));
  float getAngle();
  __attribute__((always_inline));

  // measure the noise of the sensor
  float measureNoise();

  // read dignostics
  void readDIAAGC();
  void readERRFL();

  // calculates the lookuptable with converstion from digit to deg
  void initTable(int16_t *fullsteps);

private:
  EPortType port;
  uint32_t pin;

  // private valriables
  uint16_t steps_per_revolution;
  float currentAngle;
  float lastAngle;
  float lookuptable[16384] = {};

  // private functions
  inline uint16_t getParity(uint16_t payload, uint8_t bitcount);
  inline uint16_t readRegister(uint16_t regAdress);
  inline uint16_t writeRegister(uint16_t regAdress, uint32_t content);
};

#endif
