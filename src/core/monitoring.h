//Contains the declarations for the functions used by the firmware

#ifndef __MONITORING_H__
#define __MONITORING_H__

#include <Arduino.h>

/******************************************* CLASS ************************************************/
class Monitoring_class
{
    // union that contains all relevant error bits
    typedef union MONITORING_ERROR {
        struct
        {
            volatile uint16_t COILUNDERTEMP : 1; /* bit:      0 Motor coil 0vertemp   */
            volatile uint16_t COILOVERTEMP : 1;  /* bit:      1 Motor coil 0vertemp   */
            volatile uint16_t DRIVERTEMP : 1;    /* bit:      2 Motor driver overtemp   */
            volatile uint16_t UNDERVOLTAGE : 1;  /* bit:      3 Bus voltage to low   */
            volatile uint16_t ENCODER : 1;       /* bit:      4 Encoder error  */
            volatile uint16_t EXTERNALERROR : 1; /* bit:      5 External error bit -> Error pin */
            volatile uint16_t : 10;              /* bit: 6.. 15 Reserved   */
        } bit;                                   /* Struct used for bit acces   */
        uint16_t reg;                            /* Type used for register acces */
    } MONITORING_ERROR_Type;

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
