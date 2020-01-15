



#include <Arduino.h>
#include <wiring_private.h>

#ifndef _STEPINTERFACE_H_
#define _STEPINTERFACE_H_



class stepInterface {
  public:
    // public functions
    stepInterface(uint32_t , uint32_t , uint32_t , float , float );

    // setup function
    void setup();

    // activation function
    void activate();

    // activation function
    void deactivate();

    // return function to get the current steps
    int32_t getsteps();

    // return function to get the current steps
    void readsteps(); __attribute__ ((optimize("O3")));

    // set function to set the current steps
    void writesteps(int32_t steps_);
    void writeangle(float angle_);

    // set detection rules
    void setSensetype(uint8_t sensetype_);
    void setAsync(bool async_);
    void setDebounce(bool debounce_);
    void setFilter(bool filter_);

    uint32_t getDirPin();



  private:


    float steps_per_revolution;
    float microstepping;

    // uint32_t stepPin;
    // uint32_t dirPin;

    uint8_t sensetype = EIC_CONFIG_SENSE1_RISE_Val;
    bool async = 1;
    bool debounce = 1;
    bool filter = 1;

    // EIC setup funtion
    void setupEIC();

    // TC0 setup function
    void setupTC0();

    // EVSYS setup function
    void setupEVSYS();

    // Helper Funtion to set Pins
    void setupPins();

    uint32_t getPortConfig(uint32_t pin);
    uint32_t getposConfig(uint32_t pin);


};


// Direction pin setup
void setupdirInterrupt(bool state);
void dirChanged_ISR();

#endif
