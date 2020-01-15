
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__


#include <arduino.h>



//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
extern float iMAX;




//uncomment this if you want to use a enable pin
extern float USE_ENABLE_PIN;

// microstepping setting for step input
extern float microstepping;

// fullsteps for 360 degrees
extern float steps_per_revolution;

// mm per revolution
//extern float mm_rev;

// max error in mm, if the error gets bigger the led turns off
//extern float error_led_value;

//uncomment this to invert your motor direction
extern bool INVERT;




//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
extern float M_max;

// rated current for max moment in mA
extern float I_rated;


//extern float velocity; // [deg/s]

//--------------------------------------------- Thermostor Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
extern float beta;



//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- PID Values position control -----
extern float Kp;
extern float Ki;
extern float Kd;

extern float Kf;
extern float Kv;
extern float Kac;



//----------------------------------------------- Internal Section ----------------------------------------------
//------------------------------------------------- don't change ------------------------------------------------
// baudrate speed bit/s
#define baudrate 200000000

#define firmware_version "0.4"

// frequency of the PID loop change only if you know what you're doing
#define FPID 16000.0

#endif
