
#include "SAMD51/interrupts.h"
#include "core/Controler.h"


#include "Configuration.h"

#include <SPI.h>
#include <Arduino.h>


static inline void resetTC (Tc* TCx){
  // Disable TCx
  TCx->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  WAIT_TC16_REGS_SYNC(TCx)

  // Reset TCx
  TCx->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  WAIT_TC16_REGS_SYNC(TCx)
  while (TCx->COUNT16.CTRLA.bit.SWRST);
}


void setupTCInterrupts() {
  int overflow_TC_5 = (F_CPU / FPID) - 1;

  uint16_t RangeUint16 = 65535;
  uint32_t prescalerConfigBits;
  int i = 0;

  while (overflow_TC_5  > RangeUint16) {
    i++;
    overflow_TC_5 = ((F_CPU / FPID) / (2 << (i - 1))) - 1;
  }

  if (i == 5 || i == 7 || i == 9) {//DIV32 DIV128 and DIV512 are not available
    i++;
  }
  if (i > 10) {
    i = 10;
  }

  if (i > 0) {
    overflow_TC_5 = ((F_CPU / FPID) / (2 << (i - 1))) - 1;
  }
  else {
    overflow_TC_5 = (F_CPU / FPID) - 1;
  }

  if (overflow_TC_5 > RangeUint16) {
    overflow_TC_5 = RangeUint16;
    Serial.println("Samplerate nicht möglich. Maximaler Prescaler erreicht!");
  }



  switch (i)  {
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

  // Enable GCLK For TC5 and use generator 2 as input
  GCLK->PCHCTRL[TC4_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK2_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); //use clock generator 1 (48MHz)
  GCLK->PCHCTRL[TC4_GCLK_ID].bit.GEN = 0;
  GCLK->PCHCTRL[TC4_GCLK_ID].bit.CHEN = 1;

  // wait till the generator is active
  while (GCLK->PCHCTRL[TC4_GCLK_ID].bit.CHEN == 0);

  resetTC(TC4);

 
  uint16_t tmpReg = 0;
  tmpReg |= TC_CTRLA_MODE_COUNT16;  // Set Timer counter Mode to 16 bits

  TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;  // Set TONE_TC mode as match frequency

  tmpReg |= prescalerConfigBits;
  TC4->COUNT16.CTRLA.reg |= tmpReg;


  TC4->COUNT16.CC[0].reg = (uint16_t)overflow_TC_5;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE); // wait for sync



  TC4->COUNT16.INTENSET.bit.OVF = 1; // set overflow


  NVIC_SetPriority(TC4_IRQn, 0);

  TC4->COUNT16.INTENSET.bit.MC0 = 1;
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;

  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE); // wait for sync

  // Enable InterruptVector
  NVIC_EnableIRQ(TC4_IRQn);

}


void enableTC5Interrupts() {
  // ----- enables the main PID and measuring loop -----
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC4
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE); // wait for sync
  //WAIT_TC16_REGS_SYNC(TC4)                      //wait for sync
}


void disableTC5Interrupts() {
  // ----- disables the main PID loop -----
  //resetTC(TC4);
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC4
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE); // wait for sync
  // WAIT_TC16_REGS_SYNC(TC4)                      // wait for sync
}




// ----- gets called with FPID -----
// ----- calculates the target velocity and PID settings -----
void TC4_Handler() {

  //----- Calculations -----
  if (TC4->COUNT16.INTFLAG.bit.OVF == 1 ) {  // A overflow caused the interrupt

    ControlerLoop();

    // Reset overflow flag
    TC4->COUNT16.INTFLAG.bit.MC0 = 1;
  }
}


