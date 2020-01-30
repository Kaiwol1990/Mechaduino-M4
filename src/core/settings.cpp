// Struct definition to store user settings in flash after initial compiling

#include "core/settings.h"
#include "../Configuration.h"
#include "modules/FlashStorage.h"

// create flash object to hold all 5 user settings
FlashStorage(storage, settingsBuffer_t);
FlashStorage(storageFullsteps, fullstepReadings_t);

userSettings::userSettings() : _initialized(false)
{
  // Empty
}

void userSettings::read()
{
  // check if we already initiliazed the flash
  if (!_initialized)
    init();
  settingsBuffer = storage.read();
}

void userSettings::loadSettings(uint8_t slot)
{
  // load specific slot to active values
  Serial.print("Loading user settings from slot: ");
  Serial.println(slot);
  if (!isempty(slot))
  {
    currentSettings = settingsBuffer.Slot[slot];
    updateSettings();
  }
  else
  {
    Serial.println("Slot is empty can't be loaded!");
  }
}

void userSettings::storeSettings(uint8_t slot)
{
  Serial.print("Storing user settings to slot: ");
  Serial.println(slot);
  // write specific slot to active values
  settingsBuffer.Slot[slot] = currentSettings;
}

void userSettings::setDefaultSlot(uint8_t _defaultSlot)
{
  if (_defaultSlot < 5)
  {
    defaultSlot = _defaultSlot;
    settingsBuffer.defaultSlot = _defaultSlot;
  }
}

uint8_t userSettings::getDefaultSlot()
{
  return settingsBuffer.defaultSlot;
}

bool userSettings::isValid()
{
  if (!_initialized)
    init();
  return settingsBuffer.valid;
}

void userSettings::commit()
{
  if (!_initialized)
    init();

  // check if current workspace content is different from flash content
  if (userSettings::compare())
  {
    Serial.println("Flash content is not up to date! Writing flash page!");
    settingsBuffer.valid = true;
    storage.write(settingsBuffer);
  }
  else
  {
    Serial.println("Flash has the same content as the working copys! Ignoring write!");
  }
}

// load Settings from hardcoded settings of compile time
void userSettings::loadDefaultSettings()
{
  Serial.println("Loading default settings from code");

  currentSettings.iMax = Init_iMAX;
  currentSettings.Mmax = Init_M_max;
  currentSettings.Irated = Init_I_rated;
  currentSettings.Kp = Init_Kp;
  currentSettings.Ki = Init_Ki;
  currentSettings.Kd = Init_Kd;
  currentSettings.Kf = Init_Kf;
  currentSettings.Kv = Init_Kv;
  currentSettings.Kac = Init_Kac;
  currentSettings.IntegralLimit = Init_IntegralLimit;
  currentSettings.steps_per_Revolution = Init_steps_per_revolution;
  currentSettings.microstepping = Init_microstepping;
  currentSettings.beta = Init_beta;
  currentSettings.use_enable = Init_USE_ENABLE_PIN;
  currentSettings.invert = Init_INVERT;

  updateSettings();
}

void userSettings::plotSettings(int8_t slot)
{
  settings_t workingContent;

  if (slot > -1)
  {
    workingContent = settingsBuffer.Slot[slot];
    Serial.println(" ");
    Serial.print("Settings from slot: ");
    Serial.println(slot);
    Serial.println("--------------------------");
  }
  else
  {
    workingContent = currentSettings;
    Serial.println(" ");
    Serial.println("Current settings:");
    Serial.println("--------------------------");
  }

  Serial.print("iMax = ");
  Serial.println(workingContent.iMax);

  Serial.print("Mmax = ");
  Serial.println(workingContent.Mmax);

  Serial.print("Irated = ");
  Serial.println(workingContent.Irated);

  Serial.print("Kp = ");
  Serial.println(workingContent.Kp);

  Serial.print("Ki = ");
  Serial.println(workingContent.Ki);

  Serial.print("Kd = ");
  Serial.println(workingContent.Kd);

  Serial.print("Kf = ");
  Serial.println(workingContent.Kf);

  Serial.print("Kv = ");
  Serial.println(workingContent.Kv);

  Serial.print("Kac = ");
  Serial.println(workingContent.Kac);

  Serial.print("Integrational Limit = ");
  Serial.println(workingContent.IntegralLimit);

  Serial.print("steps_per_Revolution = ");
  Serial.println(workingContent.steps_per_Revolution);

  Serial.print("microstepping = ");
  Serial.println(workingContent.microstepping);

  Serial.print("beta = ");
  Serial.println(workingContent.beta);

  Serial.print("use_enable = ");
  Serial.println(workingContent.use_enable);

  Serial.print("invert = ");
  Serial.println(workingContent.invert);
}

void userSettings::plotState()
{
  Serial.println(" ");
  Serial.println("x = slot used");
  Serial.println("c = slot changed");

  Serial.println(" ");
  Serial.println("Flash Content");
  Serial.println("Slots   | 0 | 1 | 2 | 3 | 4 |");
  Serial.println("        |-------------------|");
  Serial.print("Used    |");

  // check every entry of the buffer and flash
  for (uint8_t i = 0; i < 5; i++)
  {
    //settings_t workingContent = settingsBuffer.Slot[i];

    Serial.print(" ");
    if (compareSlot(i) == false)
    {
      {
        Serial.print("c");
      }
    }
    else
    {
      if (isempty(i) == false)
      {
        Serial.print("x");
      }
      else
      {
        Serial.print(" ");
      }
    }
    Serial.print(" |");
  }
  Serial.println(" ");

  // show default slot
  Serial.print("Default |");
  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(" ");
    if (settingsBuffer.defaultSlot != i)
    {
      Serial.print(" ");
    }
    else
    {
      Serial.print("x");
    }
    Serial.print(" |");
  }
  Serial.println(" ");
}

void userSettings::saveFullsteps(int16_t *_fullsteps)
{
  // copy readings to flash
  memcpy(fullsteps.data, _fullsteps, sizeof(_fullsteps[0]) * 200);

  fullsteps.valid = true;
  storageFullsteps.write(fullsteps);
}

bool userSettings::checkFullsteps()
{
  return fullsteps.valid;
}

//////////////////////////////////////////////////////
///// PRIVATE //////////////////////////////

void userSettings::init()
{
  //read complete flash storage object
  settingsBuffer = storage.read();
  fullsteps = storageFullsteps.read();

  _initialized = true;
}

void userSettings::updateSettings()
{
  // Calculate dependencies
  stepangle = (360.0 / (currentSettings.steps_per_Revolution * currentSettings.microstepping)); // angle of one microstep as float
  PA = 360.0 / currentSettings.steps_per_Revolution;                                            // angle of one fullstep as int
}

bool userSettings::compare()
{
  bool same[6];
  bool result;

  // read current content of flash
  settingsBuffer_t local = storage.read();

  // check every entry of the buffer and flash
  for (uint8_t i = 0; i < 5; i++)
  {
    same[i] = userSettings::compareSlot(i);
  }

  // check default slot
  same[5] = (bool)(local.defaultSlot == settingsBuffer.defaultSlot);

  result = true;
  for (uint8_t i = 0; i < 6; i++)
  {
    result &= same[i];
  }

  return !result;
}

bool userSettings::compareSlot(uint8_t _slot)
{
  settingsBuffer_t local = storage.read();
  settings_t flashContent = local.Slot[_slot];
  settings_t workingContent = settingsBuffer.Slot[_slot];

  bool same = true;
  same &= (flashContent.iMax == workingContent.iMax);
  same &= (flashContent.Mmax == workingContent.Mmax);
  same &= (flashContent.Irated == workingContent.Irated);
  same &= (flashContent.Kp == workingContent.Kp);
  same &= (flashContent.Ki == workingContent.Ki);
  same &= (flashContent.Kd == workingContent.Kd);
  same &= (flashContent.Kf == workingContent.Kf);
  same &= (flashContent.Kv == workingContent.Kv);
  same &= (flashContent.Kac == workingContent.Kac);
  same &= (flashContent.IntegralLimit == workingContent.IntegralLimit);
  same &= (flashContent.steps_per_Revolution == workingContent.steps_per_Revolution);
  same &= (flashContent.microstepping == workingContent.microstepping);
  same &= (flashContent.beta == workingContent.beta);
  same &= (flashContent.use_enable == workingContent.use_enable);
  same &= (flashContent.invert == workingContent.invert);

  return same;
}

bool userSettings::isempty(uint8_t _slot)
{
  settings_t workingContent = settingsBuffer.Slot[_slot];

  bool empty = true;
  empty &= (workingContent.iMax == 0);
  empty &= (workingContent.Mmax == 0);
  empty &= (workingContent.Irated == 0);
  empty &= (workingContent.Kp == 0);
  empty &= (workingContent.Ki == 0);
  empty &= (workingContent.Kd == 0);
  empty &= (workingContent.Kf == 0);
  empty &= (workingContent.Kv == 0);
  empty &= (workingContent.Kac == 0);
  empty &= (workingContent.IntegralLimit == 0);
  empty &= (workingContent.steps_per_Revolution == 0);
  empty &= (workingContent.microstepping == 0);
  empty &= (workingContent.beta == 0);
  empty &= (workingContent.use_enable == 0);
  empty &= (workingContent.invert == 0);

  return empty;
}