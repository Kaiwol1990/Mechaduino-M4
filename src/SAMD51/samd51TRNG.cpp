

#include "samd51TRNG.h"
/******************** CONSTRUCTOR ************************/
samd51TRNG::samd51TRNG()
{
  samd51TRNG::init();
}

/*********************** PUBLIC **************************/
// generate random integer
uint32_t samd51TRNG::getInteger()
{
  // variables
  uint32_t resultbuffer;

  // enable the generator
  TRNG->CTRLA.bit.ENABLE = 1;

  // wait for generation
  while (!TRNG->INTFLAG.bit.DATARDY)
  {
  }

  // read result
  resultbuffer = TRNG->DATA.reg;

  // clear ready flag
  TRNG->INTFLAG.bit.DATARDY = 1;

  // disable the generator
  TRNG->CTRLA.bit.ENABLE = 0;

  return resultbuffer;
}

float_t samd51TRNG::getfloat()
{
  // generate floating point number by dividing to random integers
  return ((float)samd51TRNG::getInteger() / (float)samd51TRNG::getInteger());
}

/*********************** PRIVAT **************************/
void samd51TRNG::init()
{
  // switch on Main Clock for TRNG System
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;
}