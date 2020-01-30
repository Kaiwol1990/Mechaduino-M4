

#include <Arduino.h>

#ifndef _SAMD51TC4_H_
#define _SAMD51TC4_H_

#define WAIT_TC16_REGS_SYNC(x)      \
    while (x->COUNT16.SYNCBUSY.reg) \
        ;

#define WAIT_TC_SYNC(x)                    \
    while (x->COUNT16.SYNCBUSY.bit.ENABLE) \
        ;

class samd51TC4
{

public:
    samd51TC4(int _frequency, void (*func)());

    void setup();

    void enable();

    void disable();

    void disableTC5Interrupts();

    void (*Intteruptfunc)(); // pointer to the wanted interrupt function

private:
    int frequency;

    void calcOVF();

    void resetTC(Tc *TCx);
};

#endif