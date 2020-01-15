
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__


//#include <arduino.h>


//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
#define Init_iMAX 2000

//set to 1 if you want to use a enable pin
#define Init_USE_ENABLE_PIN 1

// microstepping setting for step input
#define Init_microstepping 32

// fullsteps for 360 degrees
#define Init_steps_per_revolution 200

// mm per revolution
#define Init_mm_rev 40

//set to 1 to invert your motor direction
#define Init_INVERT 0



//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max torque in Ncm
#define Init_M_max 45.0

// rated current for max moment in mA
#define Init_I_rated 2000.0





//--------------------------------------------- Thermostor Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
#define Init_beta 3950




//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
#define Init_Kp 3.175
#define Init_Ki 29.18
#define Init_Kd 0.0

#define Init_Kf 0.3
#define Init_Kv 0.0
#define Init_Kac 0.5



//----------------------------------------------- Internal Section ----------------------------------------------
//------------------------------------------------- don't change ------------------------------------------------
// baudrate speed bit/s
#define Init_baudrate 200000000

#define firmware_version "0.4"

// frequency of the PID loop change only if you know what you're doing
#define FPID 16000.0

#endif
