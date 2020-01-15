

#include <Arduino.h>

#ifndef __SAMD51AC_H__
#define __SAMD51AC_H__

// ToDo change to enum for better scope
// Defines for the pin descrition
#define muxPinA1 0x1
#define muxPinA2 0x2
#define muxPinA3 0x0
#define muxPa07 0x3

#define muxPosVDD 0x4

#define muxNegGND 0x4
#define muxNegVDD 0x5
#define muxNegBG 0x6
#define muxNegDAC 0x7

#define MODE_SINGLE 0x1
#define MODE_CONTINOUS 0x0

#define HYST50 AC_COMPCTRL_HYST_HYST50_Val
#define HYST100 AC_COMPCTRL_HYST_HYST100_Val
#define HYST150 AC_COMPCTRL_HYST_HYST150_Val

#define FILTER0 0x0
#define FILTER3 0x1
#define FILTER5 0x2

#define SWAP_AC 0x1

// Pins
//PA04    AC/AIN[0]     A3
//PA05    AC/AIN[1]     A1
//PA06    AC/AIN[2]     A2
//PA07    AC/AIN[3]     NC ...

class samd51AC
{

public:
  samd51AC(uint8_t, uint8_t, uint8_t, uint8_t);

  void init();
  bool getState();

  void setV(float);

  void setSwap(uint8_t);
  void setHyst(uint8_t);
  void setFilter(uint8_t);

private:
  void setClock();
  void enable();
  void disable();

  void setPinmode();

  void checkPins();

  void sync(uint8_t);
  void getReady(uint8_t);

  uint8_t localChannel;
  uint8_t muxPositionNeg;
  uint8_t muxPositionPos;
  uint8_t mode;
  uint8_t swap = 0x0;
  uint8_t hyst = HYST50;
  uint8_t filter = FILTER3;
  uint8_t scaler = 0;
};

#endif
