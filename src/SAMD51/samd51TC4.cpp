
#include "SAMD51/samd51TC4.h"
//#include "core/Controler.h"
#include "core/State.h"

#include "Configuration.h"

#include <SPI.h>
#include <Arduino.h>

samd51TC4::samd51TC4(int _frequency, void (*_func)())
{
  frequency = _frequency;
  Intteruptfunc = _func;
}

void samd51TC4::setup()
{
  samd51TC4::calcOVF();
  IRQn_Type temp = TC4_IRQn;

  // set priority
  NVIC_SetPriority(temp, 0);

  // Enable InterruptVector
  NVIC_EnableIRQ(temp);
}

void samd51TC4::enable()
{
  // ----- enables the counter
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //Enable TC4
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE)
    ; // wait for sync
}

void samd51TC4::disable()
{
  // ----- disables the counter -----
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE; // Disable TC4
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE)
    ; // wait for sync
}

void TC4_Handler()
{
  if (TC4->COUNT16.INTFLAG.bit.OVF == 1)
  {
    // A overflow caused the interrupt -> Call user function
    mysamd51TC4.Intteruptfunc();

    // Reset overflow flag
    TC4->COUNT16.INTFLAG.bit.OVF = 1;
  }
}

///// PRIVATE

void samd51TC4::calcOVF()
{
  // using GENERIC_CLOCK_GENERATOR_1M as input
  uint32_t ClockFreq = 48000000L;
  //                    200000000L
  //uint32_t ClockFreq = F_CPU;

  int overflow_TC = (ClockFreq / frequency) - 1;

  uint16_t RangeUint16 = 65535;
  uint32_t prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1_Val;
  int i = 0;

  while (overflow_TC > RangeUint16)
  {
    i++;
    overflow_TC = ((ClockFreq / frequency) / (2 << (i - 1))) - 1;
  }

  if (i == 5 || i == 7 || i == 9)
  { //DIV32 DIV128 and DIV512 are not available
    i++;
  }
  if (i > 10)
  {
    i = 10;
  }

  if (i > 0)
  {
    overflow_TC = ((ClockFreq / frequency) / (2 << (i - 1))) - 1;
  }
  else
  {
    overflow_TC = (ClockFreq / frequency) - 1;
  }

  if (overflow_TC > RangeUint16)
  {
    overflow_TC = RangeUint16;
    Serial.println("Samplerate nicht möglich. Maximaler Prescaler erreicht!");
  }

  switch (i)
  {
  case 0:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1_Val;
    break;

  case 1:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV2_Val;
    break;

  case 2:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV4_Val;
    break;

  case 3:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV8_Val;
    break;

  case 4:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV16_Val;
    break;

  case 6:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV64_Val;
    break;

  case 8:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV256_Val;
    break;

  case 10:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1024_Val;
    break;
  }

  // Setup generic clock generator 7
  // use 48 MHz DFLL as clock input
  GCLK->GENCTRL[7].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) | // get source from 48 MHz Clock
                         GCLK_GENCTRL_GENEN |                          // Enable GCLK7
                         GCLK_GENCTRL_DIV(1u);                         // set divider to 1
  //                      GCLK_GENCTRL_DIV(48u);    // set divider to 48

  // Wait for synchronization
  while (GCLK->SYNCBUSY.bit.GENCTRL7)
    ;

  // enable GCLK For TCx
  // use generic clock generator 7 as input
  uint16_t GCLK_ID = TC4_GCLK_ID;
  GCLK->PCHCTRL[GCLK_ID].reg = GCLK_PCHCTRL_CHEN |
                               GCLK_PCHCTRL_GEN_GCLK7;

  // wait till the generator is active
  while (GCLK->PCHCTRL[GCLK_ID].bit.CHEN == 0)
    ;

  // reset TCx
  resetTC(TC4);

  // Set TONE_TC mode as match frequency
  TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;

  // set counter to 16 Bit mode
  TC4->COUNT16.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT16;

  // set prescaler
  TC4->COUNT16.CTRLA.bit.PRESCALER = prescalerConfigBits;

  // set overflow
  TC4->COUNT16.CC[0].reg = (uint16_t)overflow_TC;

  // wait for sync
  WAIT_TC16_REGS_SYNC(TC4);

  // enable overflow
  TC4->COUNT16.INTENSET.bit.OVF = 1;

  //TC4->COUNT16.INTENSET.bit.MC0 = 0; //1;
}

void samd51TC4::resetTC(Tc *TCx)
{
  // Disable TCx
  TCx->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  WAIT_TC16_REGS_SYNC(TCx)

  // Reset TCx
  TCx->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  WAIT_TC16_REGS_SYNC(TCx)
  while (TCx->COUNT16.CTRLA.bit.SWRST)
    ;
}