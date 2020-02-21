#ifndef __SerialMovement_H__
#define __SerialMovement_H__

#include <arduino.h>

void init_movement_menu();

void setMode();

void reportPostition();

void set_motion();

void splineMovement();

void step_response();

#endif
