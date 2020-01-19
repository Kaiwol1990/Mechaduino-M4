

#include <Arduino.h>

#ifndef _CONTROLER_H_
#define _CONTROLER_H_

class PIDControler
{

public:
  PIDControler(volatile float_t *, volatile float_t *, volatile float_t *); // * constructor.  links the PID to the Input, Output, and
  //   Setpoint. This allows for an automatic handling of the parameters

  void compute(); // Compute the output for the given Moment. Should be called with SampleTime

  void tune(); // autotune the P,I,D gains

  bool getState(); // get state of the controller

  void setState(bool); // set state of the controller

  float getError(); // get the current error

  void setSampleFreq(uint16_t); // set the sample time of the Controler

  void updateGains(); // disables the pid loop and load the current workspace settings to the controller

  void enable();
  void disable();

private:
  void reset();

  float_t myKp;  // * (P)roportional Tuning Parameter
  float_t myKi;  // * (I)ntegral Tuning Parameter
  float_t myKd;  // * (D)erivative Tuning Parameter
  float_t myKf;  // * (F)riction Tuning Parameter
  float_t myKv;  // * (V)elocitiy feedforward gain
  float_t myKac; // * (AC)celeration feedforward gain

  volatile float_t *Input;    // * Pointers to the Input, Output, and Setpoint variables
  volatile float_t *Output;   //   This creates a hard link between the variables and the
  volatile float_t *Setpoint; //   PID, freeing the user from having to constantly tell us
  //   what these values are.  with pointers we'll just know.

  float_t error;

  float_t IntegrationalTerm;
  float_t IntegrationLimit;

  float_t OutputLimit;

  float_t lastSetpoint;
  float_t lastInput;
  float_t lastVelocity;
  float_t lastAcceleration;

  uint16_t sampleFreq;
  float_t sampleTime;

  bool PIDenabled;

  float_t localOutput;
};

#endif
