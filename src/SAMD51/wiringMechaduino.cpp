

#include "wiringMechaduino.h"

extern void digitalFASTpinMode(EPortType port, uint32_t pin, uint32_t ulMode)
{

  uint32_t pinMask = (1ul << pin);

  switch (ulMode)
  {
  case INPUT:
    // Set pin to input mode
    PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN);
    PORT->Group[port].DIRCLR.reg = pinMask;
    break;

  case INPUT_PULLUP:
    // Set pin to input mode with pull-up resistor enabled
    PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
    PORT->Group[port].DIRCLR.reg = pinMask;

    // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')
    PORT->Group[port].OUTSET.reg = pinMask;
    break;

  case INPUT_PULLDOWN:
    // Set pin to input mode with pull-down resistor enabled
    PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
    PORT->Group[port].DIRCLR.reg = pinMask;

    // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
    PORT->Group[port].OUTCLR.reg = pinMask;
    break;

  case OUTPUT:
    // enable input, to support reading back values, with pullups disabled
    PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN);

    // Set pin to output mode
    PORT->Group[port].DIRSET.reg = pinMask;
    break;

  default:
    // do nothing
    break;
  }
}

void digitalFASTWrite(EPortType port, uint32_t pin, uint32_t ulVal)
{

  uint32_t pinMask = (1ul << pin);

  if (ulVal == LOW)
  {
    PORT->Group[port].OUTCLR.reg = pinMask; // write chipSelectPin LOW
  }
  else
  {
    PORT->Group[port].OUTSET.reg = pinMask; // write chipSelectPin HIGH
  }
}
