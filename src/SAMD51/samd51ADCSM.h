

#include <Arduino.h>

#ifndef __SAMD51ADCSM_H__
#define __SAMD51ADCSM_H__

class samd51ADCSM
{

public:
  samd51ADCSM();

  void init();

  void step();

  float getV();
  float getT();
  float getTCPU();

private:
  void calcLookup();
  void setPinmode();
  void setADC();

  float_t V;
  float_t T;
  float_t TCPU;

  float TL;
  float TH;

  uint32_t VCL;
  uint32_t VPL;

  uint32_t VCH;
  uint32_t VPH;

  uint32_t TP; // read ADC01 TSENSP
  uint32_t TC; // read ADC01 TSENSC

  float_t TLookup[4096];
  float_t VLookup[4096];
  uint8_t state;
};

#endif
