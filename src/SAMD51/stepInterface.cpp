

#include "stepInterface.h"
#include <wiring_private.h>

uint32_t dirPin;
uint32_t stepPin;
uint32_t enablePin;

static int32_t steps;

//steps_per_revolution( steps_per_revolution_ )
// CONSTRUCTOR AND DESTRUCTOR * * * * * * * * * * * * * * *
stepInterface_class::stepInterface_class(uint32_t stepPin_, uint32_t dirPin_, uint32_t enaPin_, float steps_per_revolution_, float microstepping_)
{
  stepPin = stepPin_;
  dirPin = dirPin_;
  enablePin = enaPin_;
  steps_per_revolution = steps_per_revolution_;
  microstepping = microstepping_;
}

// PUBLIC METHODS * * * * * * * * * * * * * * * * * * * * *

// setup function
void stepInterface_class::setup()
{
  setupPins();

  setupEIC();
  setupEVSYS();
  setupTC0();

  writesteps(0);
  activate();
}

// activation function
void stepInterface_class::activate()
{

  // enable interrupt on dirPin
  setupdirInterrupt(HIGH);

  // enable EIC to count steps on stepPin
  while (EIC->SYNCBUSY.bit.ENABLE)
    ;                        // Wait for synchronization
  EIC->CTRLA.bit.ENABLE = 1; // Enable the EIC peripheral
  while (EIC->SYNCBUSY.bit.ENABLE)
    ; // Wait for synchronization
}

// activation function
void stepInterface_class::deactivate()
{

  // disable EIC to count steps on stepPin
  while (EIC->SYNCBUSY.bit.ENABLE)
    ;                        // Wait for synchronization
  EIC->CTRLA.bit.ENABLE = 0; // Enable the EIC peripheral
  while (EIC->SYNCBUSY.bit.ENABLE)
    ; // Wait for synchronization

  // disable intterupt on dirPin
  setupdirInterrupt(LOW);
}

/*
int32_t stepInterface_class::getsteps()
{
  return steps;
}
*/
/*
// return function to get the current steps
void stepInterface_class::readsteps()
{
  TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC; // Trigger a read synchronization on the COUNT register
  while (TC0->COUNT32.SYNCBUSY.bit.CTRLB)
    ; // Wait for the CTRLB register write synchronization
  while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
    ; // Wait for the COUNT register read sychronization

  steps = TC0->COUNT32.COUNT.reg; // Update Step Variable with content of TC0 COUNT register
}*/

int32_t stepInterface_class::readsteps()
{
  TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC; // Trigger a read synchronization on the COUNT register
  while (TC0->COUNT32.SYNCBUSY.bit.CTRLB)
    ; // Wait for the CTRLB register write synchronization
  while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
    ; // Wait for the COUNT register read sychronization

  steps = TC0->COUNT32.COUNT.reg; // Update Step Variable with content of TC0 COUNT register
  return steps;
}

// set function to set the current steps
void stepInterface_class::writesteps(int32_t steps_)
{

  steps = steps_;

  TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC; // Trigger a read synchronization on the COUNT register
  while (TC0->COUNT32.SYNCBUSY.bit.CTRLB)
    ; // Wait for the CTRLB register write synchronization
  while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
    ;                             // Wait for the COUNT register read sychronization
  TC0->COUNT32.COUNT.reg = steps; // Reset the COUNT register
  while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
    ; // Wait for the COUNT register writ sychronization
}

void stepInterface_class::writeangle(float angle_)
{
  int32_t steps_ = (angle_ / (360.0 / (steps_per_revolution * microstepping))) + 0.5;

  stepInterface_class::writesteps(steps_);
}

// set detection rules
void stepInterface_class::setSensetype(uint8_t sensetype_)
{
  sensetype = sensetype_;
}

void stepInterface_class::setAsync(bool async_)
{
  async = async;
}

void stepInterface_class::setDebounce(bool debounce_)
{
  debounce = debounce;
}

void stepInterface_class::setFilter(bool filter_)
{
  filter = filter_;
}

uint32_t stepInterface_class::getDirPin()
{
  return dirPin;
}

// PRIVATE METHODS  * * * * * * * * * * * * * * * * * * * *

uint32_t stepInterface_class::getPortConfig(uint32_t pin)
{
  uint32_t config;
  EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
  pinPeripheral(stepPin, PIO_EXTINT);

  if (in > EXTERNAL_INT_7)
  {
    config = 1;
  }
  else
  {
    config = 0;
  }

  return config;
}
uint32_t stepInterface_class::getposConfig(uint32_t pin)
{

  uint32_t pos;

  EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
  pinPeripheral(stepPin, PIO_EXTINT);

  if (in > EXTERNAL_INT_7)
  {
    pos = (in - 8) << 2;
  }
  else
  {
    pos = in << 2;
  }

  return pos;
}

// EIC setup funtion
void stepInterface_class::setupEIC()
{
  uint32_t configPosition;
  uint32_t BitPosition;

  // *********** SETUP GCLK ***********
  MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC; // switch on EIC system

  // *********** SETUP EIC ***********
  configPosition = getPortConfig(stepPin); // get the valid config register for the pin (CONFIG0, CONFIG1)
  BitPosition = getposConfig(stepPin);     // get the Bit position of the EXTINTE

  EIC->CTRLA.bit.ENABLE = 0; // Disable the EIC peripheral
  while (EIC->SYNCBUSY.bit.ENABLE)
    ; // Wait for synchronization

  // *********** Writing Config Register ***********
  EIC->CONFIG[configPosition].reg |= EIC_CONFIG_SENSE0(sensetype) << BitPosition; // Set event on detecting RISING

  if (filter)
  {
    EIC->CONFIG[configPosition].reg |= EIC_CONFIG_FILTEN0 << BitPosition; // enable filter bit
  }

  if (debounce)
  {
    EIC->CONFIG[configPosition].reg |= EIC_DEBOUNCEN_DEBOUNCEN(g_APinDescription[stepPin].ulExtInt); // enable filter bit
  }

  // *********** Writing EVCTRL Register ***********
  EIC->EVCTRL.reg |= 1 << EIC_EVCTRL_EXTINTEO(g_APinDescription[stepPin].ulExtInt); // Enable event output on external interrupt

  // *********** Writing INTENCLR Register ***********
  EIC->INTENCLR.reg |= 1 << EIC_INTENCLR_EXTINT(g_APinDescription[stepPin].ulExtInt); // Clear interrupt on external interrupt

  // *********** Writing ASYNCH Register ***********
  if (async)
  {
    EIC->ASYNCH.reg |= 1 << EIC_ASYNCH_ASYNCH(g_APinDescription[stepPin].ulExtInt); // set detection to asynch
  }
}

// TC0 setup function
void stepInterface_class::setupTC0()
{
  // *********** SETUP GCLK ***********
  MCLK->APBAMASK.reg |= MCLK_APBAMASK_TC0; // Activate timer TC0

  GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |     // Enable perhipheral TC0
                                   GCLK_PCHCTRL_GEN_GCLK0; // Connect generic clock 0 to TC0

  // *********** SETUP TC ***********
  TC0->COUNT32.EVCTRL.reg = TC_EVCTRL_TCEI |       // Enable the TC event input
                            TC_EVCTRL_EVACT_COUNT; // Set up the timer to count events

  TC0->COUNT32.CTRLA.reg = TC_CTRLA_MODE_COUNT32; // Set the TC0 timer to 32-bit mode
  TC0->COUNT32.CTRLA.bit.ENABLE = 1;              // Enable the TC0 timer
  while (TC0->COUNT32.SYNCBUSY.bit.ENABLE)
    ; // Wait for synchronization
}

// EVSYS setup function
void stepInterface_class::setupEVSYS()
{
  // *********** SETUP GCLK ***********
  MCLK->APBBMASK.reg |= MCLK_APBBMASK_EVSYS; // Switch on the event system peripheral

  // *********** SETUP EVSYS ***********
  // Select the event system user on channel 0 (USER number = channel number + 1)
  EVSYS->USER[EVSYS_ID_USER_TC0_EVU].reg = EVSYS_USER_CHANNEL(1); // Set the event user (receiver) as timer TC0

  // Select the event system generator on channel 0
  // get External Interrupt Controller - Line (Doku Page 62)
  // +18 unknown doku says + 12???
  uint32_t EVSYS_ID_GEN_EIC_EXTINT_x = g_APinDescription[stepPin].ulExtInt + 18;

  EVSYS->Channel[0].CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |            // No event edge detection
                                  EVSYS_CHANNEL_PATH_ASYNCHRONOUS |               // Set event path as asynchronous
                                  EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_x); // Set event generator (sender) as external interrupt x
}

void stepInterface_class::setupPins()
{
  EPortType port;
  uint32_t pin;
  uint32_t pinMask;

  // set dirPin as INPUT Pulldown
  port = g_APinDescription[dirPin].ulPort;
  pin = g_APinDescription[dirPin].ulPin;
  pinMask = (1ul << pin);

  // Set pin to input mode with pull-down resistor enabled
  PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
  PORT->Group[port].DIRCLR.reg = pinMask;

  // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
  PORT->Group[port].OUTCLR.reg = pinMask;

  // set dirPin as INPUT Pulldown
  port = g_APinDescription[stepPin].ulPort;
  pin = g_APinDescription[stepPin].ulPin;
  pinMask = (1ul << pin);

  // Set pin to input mode with pull-down resistor enabled
  PORT->Group[port].PINCFG[pin].reg = (uint8_t)(PORT_PINCFG_INEN | PORT_PINCFG_PULLEN);
  PORT->Group[port].DIRCLR.reg = pinMask;

  // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
  PORT->Group[port].OUTCLR.reg = pinMask;
}

// Complete Public METHODS  * * * * * * * * * * * * * * * * * * * *
void setupdirInterrupt(bool state)
{

  if (state)
  {
    attachInterrupt(dirPin, dirChanged_ISR, CHANGE);
    //attachInterrupt(dirPin, dirChanged_ISR, CHANGE);
  }
  else
  {
    detachInterrupt(dirPin);
  }
}

void dirChanged_ISR()
{

  // get pin state
  //bool dir = 0;
  if ((PORT->Group[g_APinDescription[dirPin].ulPort].IN.reg & (1ul << g_APinDescription[dirPin].ulPin)) != 0)
  {
    while (TC0->COUNT32.SYNCBUSY.bit.CTRLB)
      ; // Wait for the CTRLB register write synchronization
    while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
      ;                                // Wait for the COUNT register read sychronization
    TC0->COUNT32.CTRLBCLR.bit.DIR = 1; // if direction is positive (high) set counter to count up
  }
  else
  {
    while (TC0->COUNT32.SYNCBUSY.bit.CTRLB)
      ; // Wait for the CTRLB register write synchronization
    while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
      ;                                // Wait for the COUNT register read sychronization
    TC0->COUNT32.CTRLBSET.bit.DIR = 1; // otherwise set counter to decrement
  }

  while (TC0->COUNT32.SYNCBUSY.bit.COUNT)
    ; // Wait for the COUNT register read sychronization
}
