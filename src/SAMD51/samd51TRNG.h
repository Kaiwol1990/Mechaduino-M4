

#include <Arduino.h>

#ifndef __SAMD51TRNG_H__
#define __SAMD51TRNG_H__

class samd51TRNG
{

public:
  samd51TRNG();

  uint32_t getInteger();

  float_t getfloat();

private:
  void init();
};

#endif
