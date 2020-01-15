#include "Configuration.h"




//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
float iMAX = 2000;



//set to 1 if you want to use a enable pin
float USE_ENABLE_PIN = 1;

// microstepping setting for step input
float microstepping = 32;

// fullsteps for 360 degrees
float steps_per_revolution = 200;

// mm per revolution
float mm_rev = 40;

// max error in mm, if the error gets bigger the led turns off
//float error_led_value 1.00

//set to 1 to invert your motor direction
bool INVERT = 0;



//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max torque in Ncm
float M_max = 45.0;

// rated current for max moment in mA
float I_rated = 2000.0;





//--------------------------------------------- Thermostor Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
float beta = 3950;





//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---- PID Values current control -----
// Ziegler Nichols with torque as output
float Kp = 3.175;
float Ki = 29.18;
float Kd = 0.0;

float Kf = 0.3;
float Kv = 0.0;
float Kac = 0.5;

