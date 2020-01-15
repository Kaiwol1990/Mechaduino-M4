//Contains the declarations for the functions used by the firmware

#ifndef __PLANNER_H__
#define __PLANNER_H__
#include <arduino.h>

float trapezodialSpline(float input);

float IntersectionDistance(float phi, float omega_0, float omega_final, float a_acc);

float estimaeAccAngle(float omega_0, float omega_t, float a);

float sign(float input);

float splineInterpolate(float input);

#endif
