

#include <Arduino.h>

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#if defined(__SAMD51__)
#define WAIT_TC16_REGS_SYNC(x)      \
    while (x->COUNT16.SYNCBUSY.reg) \
        ;
#else
#define WAIT_TC16_REGS_SYNC(x)             \
    while (x->COUNT16.STATUS.bit.SYNCBUSY) \
        ;
#endif

class samd51TC4
{

public:
    samd51TC4(int _frequency, void (*func)());

    void setup();

    void enable();

    void disable();

    void disableTC5Interrupts();

    void (*Intteruptfunc)(); // point to the wanted interrupt function

private:
    int frequency;

    void calcOVF();

    void resetTC(Tc *TCx);
};

#endif