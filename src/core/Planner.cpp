//Contains the definitions of the functions used by the firmware.

#include "core/objects.h"
#include "core/variables.h"
#include "core/Planner.h"
#include "../Configuration.h"

#define TOLERANCE 0.05

/******************** CONSTRUCTOR *******************/
Planner_class::Planner_class()
{
}

/******************** Public *******************/

// set the target new target and return the current interpolated target based on the motion planning
float Planner_class::setTarget(float input)
{
  // variables
  float output;

  // switch mode
  switch (Planner_class::mode)
  {
  case 0:
    output = input;
    break;

  case 1:
    output = Planner_class::trapezodialSpline(input);
    break;

  case 2:
    output = Planner_class::splineInterpolate(input);
    break;

  default:
    // default is no motion planning
    output = input;
    break;
  }

  return output;
}

void Planner_class::setMode(uint8_t mode_)
{
  Planner_class::mode = mode_;
}

uint8_t Planner_class::getMode()
{
  return Planner_class::mode;
}

/******************** PRIVAT *******************/

// Motion Planner_class for Spline interpolation.
// The Spline will always have the set max Velocity at its fastest point.
// Bevor and after this Point it will accelerate oder deccelerate
float Planner_class::splineInterpolate(float dem_val)
{
  //static float dem_val_1;
  static float dem_val_0;
  static float rate;
  static float set_1;
  static float dem_time;
  static uint32_t steps_left;

  float t_act;
  float t_m;
  float y_m;
  float y_act;
  float set;
  float diff;

  /* Calculate actual rate */
  /* Check if inputs have changed since last step */
  if (dem_val != Planner_class::dem_val_1)
  {

    diff = fabsf(Planner_class::dem_val_1 - dem_val);
    dem_time = (diff / velLimit);

    rate = (dem_val - set_1) / dem_time;

    /* Check numerical tolerance for float precision */
    if ((rate > -TOLERANCE) && (rate < TOLERANCE))
    {
      rate = (dem_val - set_1) * FPID;
      steps_left = 1;
    }
    else
    {
      steps_left = (int)(dem_time * FPID);
    }

    /* Store actual set value at begin of ramp */
    dem_val_0 = set_1;
  }
  else
  { /* Input has not changed since last step */
    if (steps_left > 0)
      /* Decrement steps left */
      steps_left -= 1;
    else
      /* Set actual rate */
      rate = (dem_val - set_1) * (float)FPID;
  }

  /* Calculate spline */
  if (steps_left > 0)
  {

    t_act = dem_time - ((float)steps_left / (float)FPID);
    t_m = dem_time / 2.0;
    y_m = (dem_val + dem_val_0) / 2.0;

    if (t_act <= t_m)
    {
      // First half of spline
      // increasing rate
      y_act = (t_act / t_m) * (t_act / t_m);
      set = (y_m - dem_val_0) * y_act + dem_val_0;
    }
    else
    {
      // Second half of spline
      // decreasing rate
      y_act = (1 - (t_act - t_m) / t_m) * (1 - (t_act - t_m) / t_m);
      set = dem_val + (y_m - dem_val) * y_act;
    }

    /* Reset actual rate */
    rate = 0.0;
  }
  else
  {
    /* Last step */
    /* Calulate final Value */
    set = (rate / FPID) + set_1;
  }

  Planner_class::dem_val_1 = dem_val;
  set_1 = set;

  return set;
}

float Planner_class::trapezodialSpline(float dem_val)
{
  // variables
  static float omega;
  static float omega_t;

  static float set;

  static float phi_1;
  static float phi_2;
  static float phi_3;

  static float a_acc;
  static float a_decc;

  static float phi;

  /* Check if inputs have changed since last step */
  if (dem_val != Planner_class::dem_val_1)
  {

    // save current target as reference
    float phi_0 = set;

    phi = dem_val - phi_0;

    if (phi < 0)
    {
      omega_t = -velLimit;
      a_acc = -accLimit;
      a_decc = accLimit;
    }
    else
    {
      omega_t = velLimit;
      a_acc = accLimit;
      a_decc = -accLimit;
    }

    // calculate distances
    float phi_acc = Planner_class::estimaeAccAngle(omega, omega_t, a_acc);
    float phi_decc = Planner_class::estimaeAccAngle(omega_t, 0, a_decc);
    float phi_full = phi - phi_acc - phi_decc;

    if (Planner_class::sign(phi) * phi_full > 0)
    {
      phi_1 = phi_0 + phi_acc;
      phi_2 = phi_0 + phi_acc + phi_full;
      phi_3 = phi_0 + phi;
    }
    else
    {
      // Nicht genug zeit um auf volle geschwindigkeit zu kommen!
      float temp = Planner_class::IntersectionDistance(phi, omega, 0, a_acc);

      phi_1 = phi_0 + temp;
      phi_2 = phi_1;
      phi_3 = phi_0 + phi;
    }

    Planner_class::dem_val_1 = dem_val;
  }

  if (Planner_class::sign(phi) > 0)
  {
    if (set < phi_1)
    {
      // accel
      omega += (a_acc / FPID);
      set += omega / FPID;
    }
    else if (set <= phi_2)
    {
      omega = omega_t;
      set += omega / FPID;
    }
    else if ((set <= phi_3) && (omega > 0))
    {
      // deccel
      omega += (a_decc / FPID);
      set += omega / FPID;
    }
    else
    {
      omega = 0;
      set = dem_val;
    }
  }
  else
  {
    if (set > phi_1)
    {
      // accel
      omega += (a_acc / FPID);
      set += omega / FPID;
    }
    else if (set >= phi_2)
    {
      omega = omega_t;
      set += omega / FPID;
    }
    else if ((set > phi_3) && (omega < 0))
    {
      // deccel
      omega += (a_decc / FPID);
      set += omega / FPID;
    }
    else
    {
      omega = 0;
      set = dem_val;
    }
  }

  return set;
}

float Planner_class::IntersectionDistance(float phi, float omega_0, float omega_final, float a_acc)
{

  return (2.0 * a_acc * phi + (omega_final * omega_final) - (omega_0 * omega_0)) / (4 * a_acc);
}

float Planner_class::estimaeAccAngle(float omega_0, float omega_t, float a)
{
  float domega = omega_t - omega_0;
  return ((omega_0 + 0.5 * domega) * domega / a);
}

float Planner_class::sign(float input)
{
  if (input >= 0)
  {
    return 1.0;
  }
  else
  {
    return -1.0;
  }
}
