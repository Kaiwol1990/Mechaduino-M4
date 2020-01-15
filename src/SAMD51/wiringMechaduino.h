



//#include <stdint.h>
#include <Arduino.h>



#ifndef _WIRING_H_
#define _WIRING_H_


extern void digitalFASTpinMode(EPortType port, uint32_t pin, uint32_t ulMode);

extern void digitalFASTWrite(EPortType port, uint32_t pin, uint32_t ulVal);









#endif 
