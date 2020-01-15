
#include "A4954.h"
#include "Lookups.h"

#include "modules/custommath.h"

#include "SAMD51/wiringMechaduino.h"
#include "SAMD51/board.h"
//#include "State.h"

#define M_Pi 3.141592653589793115997963
#define deg2rad M_Pi / 180.0

A4954::A4954(uint16_t steps_per_revolution_, uint32_t pinIN1_, uint32_t pinIN2_, uint32_t pinIN3_, uint32_t pinIN4_, uint32_t pinDAC1_, uint32_t pinDAC2_)
{
  pinDAC2 = pinDAC2_;
  pinDAC1 = pinDAC1_;
  pinIN4 = pinIN4_;
  pinIN3 = pinIN3_;
  pinIN2 = pinIN2_;
  pinIN1 = pinIN1_;
  steps_per_revolution = steps_per_revolution_;
  stepangle = 360.0 / (float)steps_per_revolution;
}

void A4954::init()
{
  EPortType port = PORTA;

  analogWriteResolution(12);

  // calculate phase multiplier
  phase_multiplier = ((10.0 * (float)steps_per_revolution) / 4.0);

  // set pin directions
  digitalFASTpinMode(port, pinIN1, OUTPUT);
  digitalFASTpinMode(port, pinIN2, OUTPUT);
  digitalFASTpinMode(port, pinIN3, OUTPUT);
  digitalFASTpinMode(port, pinIN4, OUTPUT);

  digitalFASTpinMode(port, pinDAC1, OUTPUT);
  digitalFASTpinMode(port, pinDAC2, OUTPUT);

  // write pins low
  digitalFASTWrite(port, pinIN1, LOW);
  digitalFASTWrite(port, pinIN2, LOW);
  digitalFASTWrite(port, pinIN3, LOW);
  digitalFASTWrite(port, pinIN4, LOW);

  digitalFASTWrite(port, pinDAC1, LOW);
  digitalFASTWrite(port, pinDAC2, LOW);

  //calculateLookup();
  calcLookup();
}

void A4954::setTorque(float I_rated, float M_max, float iMAX)
{
  // calculate the Gain to get from Torque to DAC digits. This is so we can use torque as the PID controller output
  // Example:
  // The given Motor has a rated torque of 45 Ncm and a ratet current of 2000mA but we limit the current to 1500mA.
  // TtoU = 82.747474 = (2000.0/45.0) * (10.0*0.15 / 1000.0) * (4096.0 / 3.3)

  TtoI = (I_rated / M_max);            // from Ncm to mA
  float ItoV = 10.0 * rSense / 1000.0; // from mA to V
  float VtoU = (4096.0 / 3.3);         // from V to digits

  ItoU = ItoV * VtoU; // as one gain

  // From the maximal allowed current we can calculate back to the maximal allow motor torque
  // Example:
  // The given Motor has a rated torque of 45 Ncm and a ratet current of 2000mA but we limit the current to 1500mA.
  // Therfore the maximal torque is 33.75 Ncm. This will be used to limit the PID controller output
  tMax = M_max * iMAX / I_rated;
}

//void A4954::output(float electric_angle, float effort, bool enabled) {
void A4954::output(float angle, float effort)
{
  float Ialpha;
  float Ibeta;
  uint32_t pole_angle;
  uint16_t lookupIdx;

  // Limit output
  if (effort < -tMax)
  {
    effort = -tMax;
  }
  else if (effort > tMax)
  {
    effort = tMax;
  }

  // calculate the electrical poleangle of the current rotor position * * * * * * * * * * * * * * * * * * * * *
  lookupIdx = ((effort * 400.0) / tMax) + 100.5;
  if (lookupIdx > 199)
  {
    lookupIdx = 199;
  }
  else if (lookupIdx < 0)
  {
    lookupIdx = 0;
  }

  pole_angle = mod(-(phase_multiplier * (angle + phaseadvancedTable[lookupIdx])), 3600);

  // get sin and cosin of the current flux angle
  float sin_phi = sin_lookup[pole_angle];
  float cos_phi = cos_lookup[pole_angle];

  Ialpha = sin_phi * abs(effort);
  Ibeta = cos_phi * abs(effort);

  // write ouptut to DACs
  writeDACs(Ialpha, Ibeta);
}

void A4954::outputOpenloop(float angle, float effort)
{
  float Ialpha;
  float Ibeta;
  uint32_t pole_angle;

  // Limit output
  if (effort < -tMax)
  {
    effort = -tMax;
  }
  else if (effort > tMax)
  {
    effort = tMax;
  }

  // calculate the electrical poleangle of the current rotor position * * * * * * * * * * * * * * * * * * * * *
  pole_angle = mod(-(phase_multiplier * angle), 3600);

  // get sin and cosin of the current flux angle
  float sin_phi = sin_lookup[pole_angle];
  float cos_phi = cos_lookup[pole_angle];

  Ialpha = sin_phi * abs(effort);
  Ibeta = cos_phi * abs(effort);

  // write ouptut to DACs
  writeDACs(Ialpha, Ibeta);
}

void A4954::writeDACs(float current_alpha, float current_beta)
{

  if (current_alpha != 0 || current_beta != 0)
  {

    if (current_alpha >= 0)
    {
      digitalFASTWrite(port, pinIN1, LOW);
      digitalFASTWrite(port, pinIN2, HIGH);
    }
    else
    {
      digitalFASTWrite(port, pinIN1, HIGH);
      digitalFASTWrite(port, pinIN2, LOW);
    }

    if (current_beta >= 0)
    {
      digitalFASTWrite(port, pinIN3, LOW);
      digitalFASTWrite(port, pinIN4, HIGH);
    }
    else
    {
      digitalFASTWrite(port, pinIN3, HIGH);
      digitalFASTWrite(port, pinIN4, LOW);
    }

    analogWrite(DAC0, abs(current_beta * TtoI * ItoU));  // VREF_2
    analogWrite(DAC1, abs(current_alpha * TtoI * ItoU)); // VREF_1
  }
  else
  {

    analogWrite(DAC0, 0);
    analogWrite(DAC1, 0);

    digitalFASTWrite(port, pinIN1, LOW);
    digitalFASTWrite(port, pinIN2, LOW);
    digitalFASTWrite(port, pinIN3, LOW);
    digitalFASTWrite(port, pinIN4, LOW);
  }
}

void A4954::calcLookup()
{
  double temp;
  for (uint16_t i = 0; i < 200; i++)
  {
    temp = tanh((((double)i - 100.0) / 100.0) * M_Pi) * 0.95 * stepangle;
    phaseadvancedTable[i] = (float)temp;
  }
}
