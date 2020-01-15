



#include <Arduino.h>

#ifndef _AS5047D_H_
#define _AS5047D_H_



class AS5047D {
  public:
    // public functions
    AS5047D(uint16_t steps_per_revolution_, Stream *,  EPortType port_, uint32_t pin_);

    // setup function
    void init(int16_t *fullsteps);

    // read angle of magnet
    uint16_t readDigits();

    void readAngle();  __attribute__ ((always_inline));
    float getAngle();  __attribute__ ((always_inline));
    float getLastAngle(); __attribute__ ((always_inline));

    // read dignostics
    void readDIAAGC();
    void readERRFL();

    void setOffset(float Offset_);
    
    // calculates the lookuptable with converstion from digit to deg
    void initTable(int16_t *fullsteps);

  private:

    EPortType port;
    uint32_t pin;
    float Offset;// = 41.44;


    // private valriables
    uint16_t steps_per_revolution;
    float currentAngle;
    float lastAngle;
    float lookuptable[16384] = {};
    // float lookuptable[4096] = {};

    // private functions
    inline uint16_t getParity(uint16_t payload, uint8_t bitcount);
    inline uint16_t readRegister(uint16_t regAdress);



};



#endif
