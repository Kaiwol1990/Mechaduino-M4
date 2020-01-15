

#include "samd51AC.h"

static bool ClockInit = false;


samd51AC::samd51AC(uint8_t muxNeg_, uint8_t muxPos_, uint8_t channel, uint8_t mode_) {

  // save pins and mode
  muxPositionNeg = muxNeg_;
  muxPositionPos = muxPos_;
  mode = mode_;

  // allocate AC Channel
  localChannel = channel;//ACCount;
}





void samd51AC::init() {


  samd51AC::setPinmode();

  samd51AC::disable();

  samd51AC::setClock();


  // *********** SETUP MODE ***********
  // 0 = cont
  // 1 = single
  AC->COMPCTRL[localChannel].bit.SINGLE = 0x0;
  while (AC->SYNCBUSY.bit.COMPCTRL0);             // Wait for synchronization


  // *********** SETUP HYSTERESIS ***********
  if (hyst != 0) {
    AC->COMPCTRL[localChannel].bit.HYSTEN = 0x1;
    samd51AC::sync(localChannel);             // Wait for synchronization
    AC->COMPCTRL[localChannel].bit.HYST = hyst;
    samd51AC::sync(localChannel);             // Wait for synchronization
  }


  // *********** SETUP SPEED MODE ***********
  //set speed -> has to be 0x3
  AC->COMPCTRL[localChannel].bit.SPEED = 0x3; // 0x3
  samd51AC::sync(localChannel);             // Wait for synchronization


  // *********** SETUP MUX POSITIONS ***********
  AC->COMPCTRL[localChannel].bit.MUXPOS = muxPositionPos;
  samd51AC::sync(localChannel);             // Wait for synchronization


  AC->COMPCTRL[localChannel].bit.MUXNEG = muxPositionNeg;
  samd51AC::sync(localChannel);             // Wait for synchronization


  // *********** ATTACHE INTTERUPT ***********
  // Interrupt if needed
  // 0: Toggle
  // 1: rising
  // 2: falling
  // 3: end of Comparison -> only single shot
  //AC->COMPCTRL[0].bit.INTSEL = AC_COMPCTRL_INTSEL(1);


  // *********** SETUP FILTER ***********
  // Filtering
  // 0: OFF
  // 1: 3-bit majority function (2 of 3)
  // 2: 5-bit majority function (3 of 5)
  AC->COMPCTRL[localChannel].bit.FLEN = filter;
  samd51AC::sync(localChannel);             // Wait for synchronization

  // *********** SETUP STANDBY OPTION ***********
  AC->COMPCTRL[localChannel].bit.RUNSTDBY = 0x0; //
  samd51AC::sync(localChannel);             // Wait for synchronization

  // *********** SETUP SWAP OPTION ***********
  // if wanted swap negative and positive input of the AC
  AC->COMPCTRL[localChannel].bit.SWAP = swap;
  samd51AC::sync(localChannel);             // Wait for synchronization



  // *********** SETUP OUTPUT ***********
  // 0: The output of COMPn is not routed to the COMPn I/O port
  // 1: The asynchronous output of COMPn is routed to the COMPn I/O port
  // 2: The synchronous output (including filtering) of COMPn is routed to the COMPn I/O port
  AC->COMPCTRL[localChannel].bit.OUT = 0x0;
  samd51AC::sync(localChannel);             // Wait for synchronization




  // *********** SETUP DAC SCALER ***********
  // if the DAC is choosen for the nagative input, set the Divider for the DAC
  AC->SCALER[localChannel].bit.VALUE = scaler;
  samd51AC::sync(localChannel);             // Wait for synchronization


  samd51AC::enable();

  samd51AC::getReady(localChannel);

}



bool samd51AC::getState() {
  if (!localChannel) {
    return AC->STATUSA.bit.STATE0;
  }
  else {
    return AC->STATUSA.bit.STATE1;
  }
}

void samd51AC::setV(float voltage) {

  samd51AC::disable();
  scaler = (((voltage / 3.3) * 64.0));
  samd51AC::enable();

}


void samd51AC::setSwap(uint8_t swap_) {
  samd51AC::disable();
  swap = swap_;
  samd51AC::enable();
}

void samd51AC::setHyst(uint8_t hyst_) {
  samd51AC::disable();
  hyst = hyst_;
  samd51AC::enable();
}

void samd51AC::setFilter(uint8_t filter_) {
  filter = filter_;
  samd51AC::enable();
}




// *********************** PRIVATE FUNCTIONS ***************************
// *********************************************************************
void samd51AC::setClock() {

  if (!ClockInit) {
    // switch on Main Clock for AC System
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_AC;

    // Connect generic clock 0 (48Mhz)?
    GCLK->PCHCTRL[AC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |         // Enable perhipheral
                                    GCLK_PCHCTRL_GEN_GCLK0;     // Connect generic clock 0 to TC0

    // save clock state -> needs only one activation
    ClockInit = true;
  }
}



void samd51AC::enable() {
  // enable comperator x
  AC->COMPCTRL[localChannel].bit.ENABLE = 0x1; // Enable AC Channel x
  samd51AC::sync(localChannel);

  // enable the whole comperator component
  AC->CTRLA.bit.ENABLE = 0x1;               // Disable the AC peripheral
  while (AC->SYNCBUSY.bit.ENABLE);          // Wait for synchronization
}



void samd51AC::disable() {
  // Disable the whole comperator component
  AC->CTRLA.bit.ENABLE = 0x0;               // Disable the AC peripheral
  while (AC->SYNCBUSY.bit.ENABLE);          // Wait for synchronization

  // disabling comperator x
  AC->COMPCTRL[localChannel].bit.ENABLE = 0x0; // Disable AC Channel x
  samd51AC::sync(localChannel);
}




void samd51AC::setPinmode() {
  // pin alwasy has to be input

  // negative input
  if (muxPositionNeg == muxPinA1) {
    pinMode(A1, INPUT);
  }
  else if (muxPositionNeg == muxPinA2) {
    pinMode(A2, INPUT);
  }
  else if (muxPositionNeg == muxPinA3) {
    pinMode(A3, INPUT);
  }

  // positive input
  if (muxPositionPos == muxPinA1) {
    pinMode(A1, INPUT);
  }
  else if (muxPositionPos == muxPinA2) {
    pinMode(A2, INPUT);
  }
  else if (muxPositionPos == muxPinA3) {
    pinMode(A3, INPUT);
  }

}




void samd51AC::checkPins() {
#if (muxPositionPos > 4)
#error "The positive pin of the comperator must be between 0 and 4"
#endif

#if (muxPositionNeg > 7)
#error "The negative pin of the comperator must be between 0 and 7"
#endif
}



// ****************** HELPER FUNCTIONS ***********************
void samd51AC::sync(uint8_t channel) {
  if (channel == 0) {
    while (AC->SYNCBUSY.bit.COMPCTRL0);
  }
  else {
    while (AC->SYNCBUSY.bit.COMPCTRL1);
  }
}


void samd51AC::getReady(uint8_t channel) {
  if (channel == 0) {
    while (AC->STATUSB.bit.READY0);
  }
  else {
    while (AC->STATUSB.bit.READY1);
  }
}
