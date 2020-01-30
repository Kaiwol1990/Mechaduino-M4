

#include <Arduino.h>

#ifndef __SAMD51TEMP_H__
#define __SAMD51TEMP_H__

class TemperatureSamd51
{
public:
  TemperatureSamd51();
  void init();
  float readInternalTemperature();
};

#endif
