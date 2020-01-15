
#include "PIDControler.h"
//#include "helperFunctions.h"
//#include "State.h"

#define M_Pi 3.141592653589793115997963

// CONSTRUCTOR AND DESTRUCTOR * * * * * * * * * * * * * * *
PIDControler::PIDControler(float_t* Input_,  float_t* Output_, float_t* Setpoint_)
{
  Input = Input_;
  Output = Output_;
  Setpoint = Setpoint_;


  OutputLimit = 10000000000000000.0;


  // calc Lookuptable
  PIDControler::calcLookup();

  // Reset Controler to make sure output is initial zero
  PIDControler::disable();
}


float PIDControler::getError() {
  return error;
}



void PIDControler::compute() {

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
  IntegrationalTerm += (Ki * error);

  if (IntegrationalTerm > IntegrationLimit) {
    IntegrationalTerm = IntegrationLimit;
  }
  else if (IntegrationalTerm < -IntegrationLimit) {
    IntegrationalTerm = -IntegrationLimit;
  }

  // compute output
  localOutput = (Kp * error) + IntegrationalTerm + (Kd * dInput) + (Kf * Jerk) + (Kv * Velocity) + (Kac * Acceleration);// + antiCoggingK;

  if (localOutput > OutputLimit) {
    localOutput = OutputLimit;
  }
  else if (localOutput < -OutputLimit) {
    localOutput = -OutputLimit;
  }


  // save Setpoint
  lastSetpoint = LocalSetpoint;
  lastInput = LocalInput;
  lastVelocity = Velocity;
  lastAcceleration = Acceleration;

  // write output

  if (PIDenabled) {
    *Output = localOutput;
  }
  else {
    PIDControler::disable();
    *Output = 0;
  }

}

void PIDControler::setIntegrationalLimit(float_t Limit_) {
  IntegrationLimit = Limit_;
}


void PIDControler::setLimit(float_t Limit_) {
  OutputLimit = Limit_;
}





void PIDControler::setTunings(float_t Kp_, float_t Ki_, float_t Kd_) {

  // scale Gains with Sampletime
  Kp = Kp_;
  Ki = Ki_ * sampleTime;
  Kd = Kd_ / sampleTime;
}



void PIDControler::setTunings(float_t Kp_, float_t Ki_, float_t Kd_, float_t Kf_, float_t Kv_, float_t Kac_) {

  // scale Gains with Sampletime
  Kp = Kp_;
  Ki = Ki_ * sampleTime;
  Kd = Kd_ / sampleTime;


  // advanced parameters
  Kf = Kf_;
  Kv = Kv_;
  Kac = Kac_;
}


bool PIDControler::getState() {
  return PIDenabled;
}



void PIDControler::setSampleFreq(uint16_t freq_) {

  sampleFreq = freq_;
  sampleTime = 1.0 / (float_t)(freq_);

}





void PIDControler::setState(bool enabled_) {
  if (enabled_) {
    PIDControler::enable();
  }
  else {
    PIDControler::disable();
  }
}



void PIDControler::enable() {
  if (!PIDenabled) {
    PIDenabled = true;

    // Reset
    PIDControler::reset();
  }
}


void PIDControler::disable() {
  if (PIDenabled) {
    PIDenabled = false;

    // Reset
    PIDControler::reset();
  }
}

void PIDControler::reset() {
  IntegrationalTerm = 0;
  localOutput = 0;

}


void PIDControler::calcLookup() {
  for (uint16_t i = 0; i < 200; i++) {
    tanhTable[i] = tanh(((double)(i - 100.0) / 100.0) * M_Pi);
  }
}



/*
void PIDControler::anticogging() {

  float coggingTarget;
  float coggingTorque[360] = {};
  float coggingPosition[360] = {};
  float coggingTorqueReverse[360] = {};
  float coggingPositionReverse[360] = {};

  PIDControler::enable();


  // Set target
  coggingTarget = 0;
  mystepInterface.writeangle(coggingTarget);
  delay(1000);


  Serial.println("Forward");
  Serial.println("Target; Position; Torque");
  for (uint16_t i = 0; i < 360; i++) {

    float target = (float)i;
    float lastTarget = target - 1.0;

    for (uint16_t j = 0; j < 10; j++) {
      mystepInterface.writeangle(lastTarget + ((float)j / 10.0));
    }

    delay(100);

    coggingTorque[i] = *Output;
    coggingPosition[i] = *Input;

    Serial.print((float)i, 4);
    Serial.print("; ");
    Serial.print(coggingPosition[i], 4);
    Serial.print("; ");
    Serial.println(coggingTorque[i], 4);
  }


  Serial.println("Reverse");
  Serial.println("Target; Position; Torque");
  for (uint16_t i = 360; i > 0; i--) {

    float target = (float)i;
    float lastTarget = target + 1.0;

    for (uint16_t j = 0; j < 10; j++) {
      mystepInterface.writeangle(lastTarget + ((float)j / 10.0));
    }
    //mystepInterface.writeangle((float)i);
    delay(100);

    coggingTorqueReverse[i] = *Output;
    coggingPositionReverse[i] = *Input;

    Serial.print((float)i, 4);
    Serial.print("; ");
    Serial.print(coggingPositionReverse[i], 4);
    Serial.print("; ");
    Serial.println(coggingTorqueReverse[i], 4);
  }



  PIDControler::disable();


  Serial.print("Cogging[360] = {");
  for (uint16_t i = 0; i < 360; i++) {
    Serial.print(coggingTorque[i], 4);
    Serial.print(" ,");
  }
  Serial.println("};");

  Serial.print("coggingPosition[360] = {");
  for (uint16_t i = 0; i < 360; i++) {
    Serial.print(coggingPosition[i], 4);
    Serial.print(" ,");
  }
  Serial.println("};");

}
*/

