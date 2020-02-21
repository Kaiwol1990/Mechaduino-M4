
#include "PIDControler.h"
#include "core/settings.h"
#include "core/objects.h"

#include "SAMD51/wiringMechaduino.h"

#define M_Pi 3.141592653589793115997963

// CONSTRUCTOR AND DESTRUCTOR * * * * * * * * * * * * * * *
PIDControler::PIDControler(volatile float_t *Input_, volatile float_t *Output_, volatile float_t *Setpoint_)
{
  Input = Input_;
  Output = Output_;
  Setpoint = Setpoint_;

  OutputLimit = 10000000000000000.0;

  // Reset Controler to make sure output is initial zero
  PIDControler::disable();
}

void PIDControler::compute()
{

  // local variables * * * * * * * * * * * * * * * * * * * * *
  float_t Velocity;
  float_t Acceleration;
  float_t Jerk;
  float_t dInput;

  float_t LocalInput;
  float_t LocalSetpoint;

  // evaluate Inputs * * * * * * * * * * * * * * * * * * * * *
  LocalSetpoint = *Setpoint;
  LocalInput = *Input;

  Velocity = (LocalSetpoint - lastSetpoint) * sampleTime;
  Acceleration = (Velocity - lastVelocity) * sampleTime;
  Jerk = (Acceleration - lastAcceleration) * sampleTime;

  dInput = LocalInput - lastInput;

  error = LocalSetpoint - LocalInput;

  // PID Terms * * * * * * * * * * * * * * * * * * * * *

  // Integral Term
  IntegrationalTerm += (myKi * error);

  if (IntegrationalTerm > IntegrationLimit)
  {
    IntegrationalTerm = IntegrationLimit;
  }
  else if (IntegrationalTerm < -IntegrationLimit)
  {
    IntegrationalTerm = -IntegrationLimit;
  }

  // compute output
  localOutput = (myKp * error) + IntegrationalTerm + (myKd * dInput) + (myKf * Jerk) + (myKv * Velocity) + (myKac * Acceleration); // + antiCoggingK;

  if (localOutput > OutputLimit)
  {
    localOutput = OutputLimit;
  }
  else if (localOutput < -OutputLimit)
  {
    localOutput = -OutputLimit;
  }

  // save Setpoint
  lastSetpoint = LocalSetpoint;
  lastInput = LocalInput;
  lastVelocity = Velocity;
  lastAcceleration = Acceleration;

  // write output

  if (PIDenabled)
  {
    *Output = localOutput;
  }
  else
  {
    PIDControler::disable();
    *Output = 0;
  }
}

bool PIDControler::getState()
{
  return PIDenabled;
}

void PIDControler::setSampleFreq(uint16_t freq_)
{

  sampleFreq = freq_;
  sampleTime = 1.0 / (float_t)(freq_);
}

float PIDControler::getError()
{
  return error;
}

void PIDControler::updateGains()
{
  // disable controller
  PIDControler::disable();

  // compute new gains based on current settings
  myKp = mySettings.currentSettings.Kp;
  myKi = mySettings.currentSettings.Ki * sampleTime;
  myKd = mySettings.currentSettings.Kd / sampleTime;

  myKf = mySettings.currentSettings.Kf;
  myKv = mySettings.currentSettings.Kv;
  myKac = mySettings.currentSettings.Kac;

  OutputLimit = mySettings.currentSettings.Mmax;
  IntegrationLimit = mySettings.currentSettings.IntegralLimit;

  // enable controller
  //PIDControler::enable();
}

void PIDControler::setState(bool enabled_)
{
  if (enabled_)
  {
    PIDControler::enable();
  }
  else
  {
    PIDControler::disable();
  }
}

void PIDControler::enable()
{
  if (!PIDenabled)
  {
    if (mysamd51ADCSM.getV() > 10.0)
    {
      PIDenabled = true;

      // Reset
      PIDControler::reset();
      digitalFASTWrite(PORTA, 9, HIGH);
    }
    else
    {
      Serial.println("Can't enable controller, motor voltage to low!");
    }
  }
}

void PIDControler::disable()
{
  if (PIDenabled)
  {
    PIDenabled = false;

    // Reset
    PIDControler::reset();
    digitalFASTWrite(PORTA, 9, LOW);
  }
}

void PIDControler::reset()
{
  IntegrationalTerm = 0;
  localOutput = 0;
}
