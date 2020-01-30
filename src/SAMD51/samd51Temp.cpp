

#include "SAMD51/samd51Temp.h"
#include "SAMD51/board.h"
#include "core/State.h"

/*
  TemperatureZero.h - Arduino library for internal temperature of the family SAMD -
  Copyright (c) 2018 Electronic Cats.  All right reserved.
  Based in the work of Mitchell Pontague https://github.com/arduino/ArduinoCore-samd/pull/277
*/

#include "Arduino.h"

#define NVMCTRL_TEMP_LOG NVMCTRL_TEMP_LOG_W0

TemperatureSamd51::TemperatureSamd51()
{
}

void TemperatureSamd51::init()
{
  // set power supply to ondemand mode
  SUPC->VREF.bit.ONDEMAND = 0x1;

  // activate temperature sensor
  SUPC->VREF.bit.TSEN = 1;
  SUPC->VREF.bit.VREFOE = 1;
}

// Reads temperature using internal ADC0 channel
// Datasheet chapter 37.10.8 - Temperature Sensor Characteristics
float TemperatureSamd51::readInternalTemperature()
{

  uint32_t TP; // read ADC1 TSENSP
  uint32_t TC; // read ADC1 TSENSC
  // Save ADC0 settings
  //uint16_t oldReadResolution = ADC0->CTRLB.reg;
  //uint16_t oldSampling = ADC0->SAMPCTRL.reg;
  //uint16_t oldReferenceGain = ADC0->INPUTCTRL.bit.GAIN;
  //uint16_t oldReferenceSelect = ADC0->REFCTRL.bit.REFSEL;

  // set Ref to 1 V
  analogReference(AR_INTERNAL1V0);

  // Set to 12 bits resolution
  ADC0->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT | ADC_CTRLA_PRESCALER_DIV256;
  // Wait for synchronization of registers between the clock domains
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Ensure we are sampling slowly
  ADC0->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(0x3f);
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Select MUXPOS as PTAT channel, and MUXNEG  as internal ground
  ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_PTAT;
  ADC0->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Enable ADC0
  ADC0->CTRLA.bit.ENABLE = 1;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Start ADC0 conversion
  ADC0->SWTRIG.bit.START = 1;
  // Clear the Data Ready flag
  ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Start conversion again, since The first conversion after the reference is changed must not be used.
  ADC0->SWTRIG.bit.START = 1;

  // Wait until ADC0 conversion is done
  while (!(ADC0->INTFLAG.bit.RESRDY))
    ;

  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Get result
  // This is signed so that the math later is done signed
  TP = ADC0->RESULT.reg;
  // Clear result ready flag
  ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Disable ADC0
  ADC0->CTRLA.bit.ENABLE = 0;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;

  // Read CTAT

  // Select MUXPOS as PTAT channel, and MUXNEG  as internal ground
  ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_CTAT;
  ADC0->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Enable ADC0
  ADC0->CTRLA.bit.ENABLE = 1;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Start ADC0 conversion
  ADC0->SWTRIG.bit.START = 1;
  // Clear the Data Ready flag
  ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Start conversion again, since The first conversion after the reference is changed must not be used.
  ADC0->SWTRIG.bit.START = 1;
  // Wait until ADC0 conversion is done
  while (!(ADC0->INTFLAG.bit.RESRDY))
    ;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Get result
  // This is signed so that the math later is done signed
  TC = ADC0->RESULT.reg;
  // Clear result ready flag
  ADC0->INTFLAG.reg = ADC_INTFLAG_RESRDY;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  // Disable ADC0
  ADC0->CTRLA.bit.ENABLE = 0;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;

  // set VREF back to external -> jumper to 3.3 Volt
  analogReference(AR_EXTERNAL);

  /*
  // Restore pervious ADC0 settings
  ADC0->CTRLB.reg = oldReadResolution;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  ADC0->SAMPCTRL.reg = oldSampling;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
  ADC0->INPUTCTRL.bit.GAIN = oldReferenceGain;
  ADC0->REFCTRL.bit.REFSEL = oldReferenceSelect;
  while (ADC0->SYNCBUSY.reg & ADC_SYNCBUSY_ENABLE)
    ;
*/

  // Factory low temperature readings
  uint8_t TLI = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_INT_ADDR & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos; // integer
  uint8_t TLD = (*(uint32_t *)FUSES_ROOM_TEMP_VAL_DEC_ADDR & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos; // decimal
  int32_t TL = 1000 * TLI + 100 * TLD;

  // Factory high temperature reading
  uint8_t THI = (*(uint32_t *)FUSES_HOT_TEMP_VAL_INT_ADDR & FUSES_HOT_TEMP_VAL_INT_Msk) >> FUSES_HOT_TEMP_VAL_INT_Pos; // integer
  uint8_t THD = (*(uint32_t *)FUSES_HOT_TEMP_VAL_DEC_ADDR & FUSES_HOT_TEMP_VAL_DEC_Msk) >> FUSES_HOT_TEMP_VAL_DEC_Pos; // decimal
  int32_t TH = 1000 * THI + 100 * THD;

  // get PTAT and CTAT ADC factory Values
  uint32_t VCL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_CTAT_ADDR & FUSES_ROOM_ADC_VAL_CTAT_Msk) >> FUSES_ROOM_ADC_VAL_CTAT_Pos;
  uint32_t VPL = (*(uint32_t *)FUSES_ROOM_ADC_VAL_PTAT_ADDR & FUSES_ROOM_ADC_VAL_PTAT_Msk) >> FUSES_ROOM_ADC_VAL_PTAT_Pos;

  uint32_t VCH = (*(uint32_t *)FUSES_HOT_ADC_VAL_CTAT_ADDR & FUSES_HOT_ADC_VAL_CTAT_Msk) >> FUSES_HOT_ADC_VAL_CTAT_Pos;
  uint32_t VPH = (*(uint32_t *)FUSES_HOT_ADC_VAL_PTAT_ADDR & FUSES_HOT_ADC_VAL_PTAT_Msk) >> FUSES_HOT_ADC_VAL_PTAT_Pos;

  //ADC.INPUTCTRL.MUXPOS setzen und entsprechende Werte lesen

  float_t T = ((TL * VPH * TC) - (VPL * TH * TC) - (TL * VCH * TP) + (TH * VCL * TP)) / ((VCL * TP) - (VCH * TP) - (VPL * TC) + (VPH * TC));

  return T / 1000.0f;
}