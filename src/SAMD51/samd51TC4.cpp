
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

  // set priority
  NVIC_SetPriority(TC4_IRQn, 0);

  // Enable InterruptVector
  NVIC_EnableIRQ(TC4_IRQn);
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
  int overflow_TC = (F_CPU / frequency) - 1;

  uint16_t RangeUint16 = 65535;
  uint32_t prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1;
  int i = 0;

  while (overflow_TC > RangeUint16)
  {
    i++;
    overflow_TC = ((F_CPU / frequency) / (2 << (i - 1))) - 1;
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
    overflow_TC = ((F_CPU / frequency) / (2 << (i - 1))) - 1;
  }
  else
  {
    overflow_TC = (F_CPU / frequency) - 1;
  }

  if (overflow_TC > RangeUint16)
  {
    overflow_TC = RangeUint16;
    Serial.println("Samplerate nicht möglich. Maximaler Prescaler erreicht!");
  }

  switch (i)
  {
  case 0:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1;
    break;

  case 1:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV2;
    break;

  case 2:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV4;
    break;

  case 3:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV8;
    break;

  case 4:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV16;
    break;

  case 6:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV64;
    break;

  case 8:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV256;
    break;

  case 10:
    prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1024;
    break;
  }

  // ATSAMD51
  // Setup Generic Clock 7 to receive 120 MHz
  /*
  GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(1) |   // Divide the 120MHz clock source by divisor 1 -> 120MHz
                         GCLK_GENCTRL_IDC |      // Set the duty cycle to 50/50 HIGH/LOW
                         GCLK_GENCTRL_GENEN |    // Enable GCLK7
                         GCLK_GENCTRL_SRC_DPLL0; // Generate from DFLL0 clock source (runs with F_CPU)
  while (GCLK->SYNCBUSY.bit.GENCTRL7)
    ; // Wait for synchronization
    */

  // Main Clock gets feed by PLL0
  // PLL0 is setup with GCLK5 at 1 MHz
  // Divider for PLL0 is set to nearest full MHz value -> 120MHz -> divider = 119.5

  // Enable GCLK For TC5 and use generator 0 as input -> Main GCLK is running with CPU speed
  GCLK->PCHCTRL[TC4_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |
                                   GCLK_PCHCTRL_GEN_GCLK0;
  GCLK->PCHCTRL[TC4_GCLK_ID].bit.GEN = 0;
  GCLK->PCHCTRL[TC4_GCLK_ID].bit.CHEN = 1;

  // wait till the generator is active
  while (GCLK->PCHCTRL[TC4_GCLK_ID].bit.CHEN == 0)
    ;

  resetTC(TC4);

  uint16_t tmpReg = 0;
  tmpReg |= TC_CTRLA_MODE_COUNT16; // Set Timer counter Mode to 16 bits

  TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ; // Set TONE_TC mode as match frequency

  tmpReg |= prescalerConfigBits;
  TC4->COUNT16.CTRLA.reg |= tmpReg;

  TC4->COUNT16.CC[0].reg = (uint16_t)overflow_TC;

  WAIT_TC16_REGS_SYNC(TC4);
  // while (TC4->COUNT16.SYNCBUSY.bit.ENABLE)
  //   ; // wait for sync

  TC4->COUNT16.INTENSET.bit.OVF = 1; // set overflow

  TC4->COUNT16.INTENSET.bit.MC0 = 0; //1;
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