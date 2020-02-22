//Contains the declarations for the functions used by the firmware

#ifndef __PLANNER_H__
#define __PLANNER_H__

class Planner_class
{
public:
    Planner_class();

    float setTarget(float); /* sets new target based on current movement mode */

    void setMode(uint8_t); /* switch motion planning mode */

    uint8_t getMode(); /* switch motion planning mode */

private:
    uint8_t mode;
    float dem_val_1;

    float trapezodialSpline(float input);

    float splineInterpolate(float input);

    float IntersectionDistance(float phi, float omega_0, float omega_final, float a_acc); /* helper function: */

    float estimaeAccAngle(float omega_0, float omega_t, float a); /* helper function: */

    float sign(float input); /* helper function: */
};
#endif
