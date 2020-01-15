

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#if defined(__SAMD51__)
#define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.SYNCBUSY.reg);
#else
#define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.STATUS.bit.SYNCBUSY);
#endif



void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();


//void TC4_Handler() __attribute__ ((optimize("O3")));


#endif