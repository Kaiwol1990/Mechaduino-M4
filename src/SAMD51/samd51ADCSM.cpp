

#include "SAMD51/samd51ADCSM.h"
#include "SAMD51/board.h"
#include "core/State.h"

#define NVMCTRL_TEMP_LOG NVMCTRL_TEMP_LOG_W0

samd51ADCSM::samd51ADCSM()
{

  // set State
  state = 0;
}

void samd51ADCSM::init()
{

  samd51ADCSM::setPinmode();

  samd51ADCSM::setADC();

  samd51ADCSM::calcLookup();
}

void samd51ADCSM::step()
{
  switch (samd51ADCSM::state)
  {

  // Coil Temperature
  case 0:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL))
    {                                                             // wait for sync
      ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN7_Val; // Selection for the positive ADC input -> AIN7
      samd51ADCSM::state++;
    }
    break;

  case 1:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x01; // Enable ADC
      samd51ADCSM::state++;
    }
    break;

  case 2:
    // Start conversion
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->SWTRIG.bit.START = 0x01; // start

      ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY; // Clear the Data Ready flag

      ADC0->SWTRIG.bit.START = 0x01; // Start conversion again, since The first conversion after the reference is changed must not be used.
      samd51ADCSM::state++;
    }
    break;

  case 3:
    if (ADC0->INTFLAG.bit.RESRDY != 0)
    {                                             // Waiting for conversion to complete
      T = samd51ADCSM::TLookup[ADC0->RESULT.reg]; // convert to temperature

      samd51ADCSM::state++;
    }
    break;

  case 4:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x00; // Disable ADC
      samd51ADCSM::state++;
    }
    break;

  case 5:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    { // wait for sync
      samd51ADCSM::state++;
    }

    break;

  // Motor Voltage
  case 6:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL))
    {                                                              // wait for sync
      ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN10_Val; // Selection for the positive ADC input -> AIN10
      samd51ADCSM::state++;
    }
    break;

  case 7:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x01; // Enable ADC
      samd51ADCSM::state++;
    }
    break;

  case 8:
    // Start conversion
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->SWTRIG.bit.START = 0x01; // start

      ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY; // Clear the Data Ready flag

      ADC0->SWTRIG.bit.START = 0x01; // Start conversion again, since The first conversion after the reference is changed must not be used.
      samd51ADCSM::state++;
    }
    break;

  case 9:
    if (!(ADC0->INTFLAG.bit.RESRDY == 0))
    {                                             // Waiting for conversion to complete
      V = samd51ADCSM::VLookup[ADC0->RESULT.reg]; // convert to temperature

      samd51ADCSM::state++;
    }
    break;

  case 10:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x00; // Disable ADC
      //samd51ADCSM::state++;
      samd51ADCSM::state = 0;
    }
    break;

    // CPU Temp
  case 11:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL))
    {                                                         // wait for sync
      ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_PTAT; // Selection for the positive ADC input -> AIN7
      samd51ADCSM::state++;
    }
    break;

  case 12:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x01; // Enable ADC
      samd51ADCSM::state++;
    }
    break;

  case 13:
    break;

  case 14:
    // Start conversion
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->SWTRIG.bit.START = 0x01; // start

      ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY; // Clear the Data Ready flag

      ADC0->SWTRIG.bit.START = 0x01; // Start conversion again, since The first conversion after the reference is changed must not be used.
      samd51ADCSM::state++;
    }
    break;

  case 15:
    if (ADC0->INTFLAG.bit.RESRDY != 0)
    { // Waiting for conversion to complete
      samd51ADCSM::TP = ADC0->RESULT.reg;
      samd51ADCSM::state++;
    }
    break;

  case 16:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x00; // Disable ADC
      samd51ADCSM::state++;
    }
    break;

  case 17:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    { // wait for sync
      samd51ADCSM::state++;
    }

    break;
  case 18:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL))
    {                                                         // wait for sync
      ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_CTAT; // Selection for the positive ADC input -> AIN7
      samd51ADCSM::state++;
    }
    break;

  case 19:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x01; // Enable ADC
      samd51ADCSM::state++;
    }
    break;

  case 20:
    break;

  case 21:
    // Start conversion
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->SWTRIG.bit.START = 0x01; // start

      ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY; // Clear the Data Ready flag

      ADC0->SWTRIG.bit.START = 0x01; // Start conversion again, since The first conversion after the reference is changed must not be used.
      samd51ADCSM::state++;
    }
    break;

  case 22:
    if (ADC0->INTFLAG.bit.RESRDY != 0)
    { // Waiting for conversion to complete
      samd51ADCSM::TC = ADC0->RESULT.reg;
      samd51ADCSM::state++;
    }
    break;

  case 23:
    if (!(ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE))
    {                                // wait for sync
      ADC0->CTRLA.bit.ENABLE = 0x00; // Disable ADC
      samd51ADCSM::state++;
    }
    break;

  case 24:
    samd51ADCSM::TCPU = ((TL * VPH * samd51ADCSM::TC) - (VPL * TH * samd51ADCSM::TC) - (samd51ADCSM::TL * VCH * samd51ADCSM::TP) + (samd51ADCSM::TH * VCL * samd51ADCSM::TP)) / ((VCL * samd51ADCSM::TP) - (VCH * samd51ADCSM::TP) - (VPL * samd51ADCSM::TC) + (VPH * samd51ADCSM::TC));
    samd51ADCSM::state = 0;
    break;
  }
}

float_t samd51ADCSM::getV()
{
  return samd51ADCSM::V;
}

float_t samd51ADCSM::getT()
{
  return samd51ADCSM::T;
}

float_t samd51ADCSM::getTCPU()
{
  return samd51ADCSM::TCPU;
}

// *********************** PRIVATE FUNCTIONS ***************************
// *********************************************************************

void samd51ADCSM::calcLookup()
{
  // Motorvoltage lookup
  for (uint16_t i = 0; i < 4096; i++)
  {
    VLookup[i] = 3.3 * 106.8 * (float)i / (6.8 * 4096.0);
  }

  // Themistor Lookuptable
  float SERIESRESISTOR = 100000;
  float THERMISTORNOMINAL = 100000;
  float valueRead;
  float temp;

  for (uint16_t i = 0; i < 4096; i++)
  {

    // Calculate the resistance of the thermistor
    valueRead = (4095.0 / i) - 1.0;
    valueRead = SERIESRESISTOR / valueRead;

    // Inverse Steinhart equation
    temp = valueRead / THERMISTORNOMINAL;
    temp = log(temp);
    temp = temp / mySettings.currentSettings.beta;
    temp = temp + (1.0 / (25.0 + 273.15));
    temp = 1.0 / temp;

    TLookup[i] = temp - 273.15;
  }

  // Factory low temperature readings
  uint8_t TLI = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_INT_ADDR & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos; // integer
  uint8_t TLD = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_DEC_ADDR & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos; // decimal
  TL = TLI + 0.1 * TLD;

  // Factory high temperature reading
  uint8_t THI = (*(uint32_t *)FUSES_HOT_TEMP_VAL_INT_ADDR & FUSES_HOT_TEMP_VAL_INT_Msk) >> FUSES_HOT_TEMP_VAL_INT_Pos; // integer
  uint8_t THD = (*(uint32_t *)FUSES_HOT_TEMP_VAL_DEC_ADDR & FUSES_HOT_TEMP_VAL_DEC_Msk) >> FUSES_HOT_TEMP_VAL_DEC_Pos; // decimal
  TH = THI + 0.1 * THD;

  // get PTAT and CTAT ADC0 factory Values
  VCH = (*(uint32_t *)FUSES_HOT_ADC_VAL_CTAT_ADDR & FUSES_HOT_ADC_VAL_CTAT_Msk) >> FUSES_HOT_ADC_VAL_CTAT_Pos;
  VCL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_CTAT_ADDR & FUSES_ROOM_ADC_VAL_CTAT_Msk) >> FUSES_ROOM_ADC_VAL_CTAT_Pos;

  VPH = (*(uint32_t *)FUSES_HOT_ADC_VAL_PTAT_ADDR & FUSES_HOT_ADC_VAL_PTAT_Msk) >> FUSES_HOT_ADC_VAL_PTAT_Pos;
  VPL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_PTAT_ADDR & FUSES_ROOM_ADC_VAL_PTAT_Msk) >> FUSES_ROOM_ADC_VAL_PTAT_Pos;
}

void samd51ADCSM::setPinmode()
{
  // Set Pins to input
  REG_PORT_DIRSET1 &= TH_pin;
  REG_PORT_DIRSET1 &= V_pin;

  // activate temperature sensor
  SUPC->VREF.bit.TSEN = 1;
  SUPC->VREF.bit.VREFOE = 1;
}

void samd51ADCSM::setADC()
{
  ADC0->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_12BIT_Val;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_CTRLB)
    ; //wait for sync
}