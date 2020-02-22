
#include <Arduino.h>

#ifndef __A4954_H__
#define __A4954_H__

//void output(float electric_angle, float effort) __attribute__ ((optimize("O3")));

class A4954_class
{
public:
  A4954_class(uint32_t pinIN1, uint32_t pinIN2, uint32_t pinIN3, uint32_t pinIN4, uint32_t pinDAC1, uint32_t pinDAC2);

  // setup function
  void init();

  // set torque gain
  void setTorque(float I_rated, float M_max, float iMax);

  // setOutput
  void output(float, float);
  __attribute__((optimize("O3")));
  void outputOpenloop(float, float);
  __attribute__((optimize("O3")));

  void writeDACs(float current_alpha, float current_beta);

  void calcLookup();

private:
  EPortType port;
  uint32_t pinIN1;
  uint32_t pinIN2;
  uint32_t pinIN3;
  uint32_t pinIN4;
  uint32_t pinDAC1;
  uint32_t pinDAC2;

  float phase_multiplier;
  float rSense = 0.150;
  float ItoU;
  float TtoI;
  float tMax;

  float phaseadvancedTable[200];
};

#endif
