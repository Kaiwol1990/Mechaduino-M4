#ifndef __SerialMovement_H__
#define __SerialMovement_H__

#include <arduino.h>

void set_motion(int arg_cnt, char **args);

void splineMovement(int arg_cnt, char **args);

void step_response(int arg_cnt, char **args);

#endif
