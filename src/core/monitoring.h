//Contains the declarations for the functions used by the firmware

#ifndef __MONITORING_H__
#define __MONITORING_H__

#include <Arduino.h>

/********************************* Typedefs and struct definitions *********************************/
typedef union {
    struct
    {
        volatile uint8_t COILTEMP : 1;      /* bit:      0 Motor Coil Overtemp   */
        volatile uint8_t DRIVERTEMP : 1;    /* bit:      1 Motor Coil Overtemp   */
        volatile uint8_t UNDERVOLTAGE : 1;  /* bit:      2 Motor Coil Overtemp   */
        volatile uint8_t ENCODER : 1;       /* bit:      3 Motor Coil Overtemp   */
        volatile uint8_t : 3;               /* bit:  4.. 6 Motor Coil Overtemp   */
        volatile uint8_t EXTERNALERROR : 1; /* bit:      7 Motor Coil Overtemp   */
    } bit;                                  /* Struct used for bit acces   */
    uint8_t reg;                            /* Type used for register acces */
} MONITORING_ERROR_Type;

/*
typedef struct
{
    volatile MONITORING_ERROR_Type ERROR;
} MONITORING_Type;
*/

/******************************************* CLASS ************************************************/
class Monitoring_class
{
public:
    // variables
    MONITORING_ERROR_Type ERROR;

    // constructor
    Monitoring_class();

    // functions
    void check();

    void printError();

    //
    bool errorOccured();

private:
};
#endif
