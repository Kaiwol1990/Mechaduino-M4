//Contains the declarations for the functions used by the firmware

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <Arduino.h>

// struct of one single user setting slot
typedef struct
{
    // controller / user settings
    float Mmax;
    float Kp;
    float Ki;
    float Kd;
    float Kf;
    float Kv;
    float Kac;
    float IntegralLimit;
    uint16_t microstepping;

    // hardware settings
    uint16_t beta;
    float iMax;
    float Irated;
    uint16_t steps_per_Revolution;
    bool use_enable;
    bool invert;

    // flag to check if setting is valid
    bool valid;
} settings_t;

// struct that holds fullstep readings
typedef struct
{
    int16_t data[200];
    bool valid;
} fullstepReadings_t;

// struct that holds all setting slots combined
typedef struct
{
    settings_t Slot[5];
    bool valid;
    uint8_t defaultSlot;
} settingsBuffer_t;

class userSettings
{

public:
    userSettings();

    // load Settings from hardcoded settings of compile time
    void loadDefaultSettings();

    // read the hole buffer with 5 slots from flash
    void read();

    // load Settings from user specific slot
    // slots are available from 0 to 4
    void loadSettings(uint8_t slot);

    // store Settings in user specific slot
    // slots are available from 0 to 4
    void storeSettings(uint8_t slot);

    void setDefaultSlot(uint8_t _defaultSlot);
    uint8_t getDefaultSlot();

    // write the hole buffer to the flash
    void write(uint8_t _slot);

    void update(settings_t _newSettings);

    bool isValid();

    void commit();

    void plotSettings(int8_t slot);

    void plotState();

    void saveFullsteps(int16_t *_fullsteps);

    bool checkFullsteps();

    /* Holds the angle information of the fullsteps */
    fullstepReadings_t fullsteps;

    // Holds the current user settings that are active
    settings_t currentSettings;

    // the default slot that gets loaded on system boot
    uint8_t defaultSlot;
    float stepangle;
    float PA;

private:
    // Variables
    bool _initialized;

    //Initialize the Settingsbuffer by reading it from the flash.

    void init();

    // Copys the given struct into the current workspace settings and calculates dependecies
    void updateSettings();

    //compares the content of the flash with the current settingsBuffer
    bool compare();

    //compares a single slot of the settings buffer with the corresponding slot in the flash memory
    bool compareSlot(uint8_t _slot);

    //hecks if the given slot ist empty (all zeros)
    bool isempty(uint8_t _slot);

    /*
      Struct that holds the current user settings gets initialized empty and can be
      loaded from flash or from hardcoded default settings the flash can hold up to
      5 user settings. To load from flash use loadSettings(). To save to flash use
      storeSettings(). Default can be loaded with loadDefaultSettings(). On boot
      settings in slot 0 will be loaded if available.
    */

    // Holds the content of all 5 user settings
    settingsBuffer_t settingsBuffer;
};

#endif