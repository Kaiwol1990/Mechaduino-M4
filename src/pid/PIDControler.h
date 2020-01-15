

#include <Arduino.h>

#ifndef _CONTROLER_H_
#define _CONTROLER_H_


class PIDControler {

  public:


    PIDControler(float_t*,  float_t*, float_t*);	// * constructor.  links the PID to the Input, Output, and
    //   Setpoint. This allows for an automatic handling of the parameters

    void compute();															// Compute the output for the given Moment. Should be called with SampleTime

    void setIntegrationalLimit(float_t);                // Set the Limit of the Integrational Term

    void setLimit(float_t);               // Set the Limit of the Integrational Term

    void setTunings(float_t, float_t, float_t);	                // Set the Tuning Parameters for the Controler

    void setTunings(float_t, float_t, float_t, float_t, float_t, float_t);  // overload to set kf, kv, kac

    void setSampleFreq(uint16_t);								// set the sample time of the Controler

    void setDir(bool);                         // poibility to invert the controler Output

    void tune(); 															// autotune the P,I,D gains

    bool getState();

    float getError();


    void enable();
    void disable();
    void setState(bool);



  private:
    void calcLookup();
    void reset();


    float_t Kp;          // * (P)roportional Tuning Parameter
    float_t Ki;          // * (I)ntegral Tuning Parameter
    float_t Kd;          // * (D)erivative Tuning Parameter
    float_t Kf;          // * (F)riction Tuning Parameter
    float_t Kv; 				 // * (V)elocitiy feedforward gain
    float_t Kac; 				 // * (AC)celeration feedforward gain


    float_t kl;                 // anti windup LImit


    float_t *Input;              // * Pointers to the Input, Output, and Setpoint variables
    float_t *Output;             //   This creates a hard link between the variables and the
    float_t *Setpoint;           //   PID, freeing the user from having to constantly tell us
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


    float_t tanhTable[200]; 	// Lookup Table for tanh values. Used to apply friction compensation


};


#endif
