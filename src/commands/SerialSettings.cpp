
#include "commands/SerialSettings.h"
#include "core/objects.h"
#include "modules/Cmd.h"

#include "language/en.h"

void init_settings_menu()
{
  // generates the commands and dependencies for this "submenu"
  Commander.cmdAdd("settings", "menu for handle settings", setting_menu);
}

//void setting_menu()
void setting_menu()
{
  Serial.println(" ");
  Serial.println("Settings menu");
  Serial.println("-------------");
  //if (Commander.check_argument(args, arg_cnt, help_subcmd))
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Menu to store oder load user settings in the flash of the SAMD51.");
    Serial.println(" ");
    Serial.println("-load [x]       loads the settings stored in the given slot (0-4). If no slot is given the default slot will be used.");
    Serial.println("-store [x]      stores the current settings in the given slot (0-4). If no slot is given the default slot will be used.");
    Serial.println("-setdefault [x] sets the given slot to the default slot.");
    Serial.println("-show [x]       prints the content of the given slot. IF no slot is given the current active settings will be printed.");
    Serial.println("-restore        restores the hardcoded factory settings to the current settings");
    Serial.println("-save           saves all settings to the flash. If a change can be found");
    Serial.println("-state          shows the state of the 5 user setting slots");
    Serial.println(" ");
    return;
  }

  // Load settings from specific slot to the active uses settings. If no slot is given the default slot will be used
  //if (Commander.check_argument(args, arg_cnt, "-load"))
  if (Commander.check_argument("-load"))
  {
    //int8_t slot_load = Commander.return_integer_argument(args, arg_cnt, "-load", Settings.getDefaultSlot(), Settings.getDefaultSlot(), 4);
    int8_t slot_load = Commander.return_integer_argument("-load", Settings.getDefaultSlot(), Settings.getDefaultSlot(), 4);
    Settings.loadSettings(slot_load);
  }

  // store current settings to specific slot. If no slot is given the default slot will be used
  //if (Commander.check_argument(args, arg_cnt, "-store"))
  if (Commander.check_argument("-store"))
  {
    //int8_t slot_store = Commander.return_integer_argument(args, arg_cnt, "-store", Settings.getDefaultSlot(), Settings.getDefaultSlot(), 4);
    int8_t slot_store = Commander.return_integer_argument("-store", Settings.getDefaultSlot(), Settings.getDefaultSlot(), 4);
    Settings.storeSettings(slot_store);
  }

  // set default slot to given slot
  // int8_t slot_default = Commander.return_integer_argument(args, arg_cnt, "-setdefault", -1, -1, 4);
  int8_t slot_default = Commander.return_integer_argument("-setdefault", -1, -1, 4);
  if (slot_default > -1)
  {
    Settings.setDefaultSlot(slot_default);
  }

  // print content of specific slot
  //if (Commander.check_argument(args, arg_cnt, "-show"))
  if (Commander.check_argument("-show"))
  {
    //int8_t slot_print = Commander.return_integer_argument(args, arg_cnt, "-show", -1, -1, 4);
    int8_t slot_print = Commander.return_integer_argument("-show", -1, -1, 4);
    Settings.plotSettings(slot_print);
  }

  // restore current settings to default settings
  //if (Commander.check_argument(args, arg_cnt, "-restore"))
  if (Commander.check_argument("-restore"))
  {
    Settings.loadDefaultSettings();
  }

  // save current settings (all slots) to flash
  if (Commander.check_argument("-save"))
  {
    Settings.commit();
  }

  // show state of the different slots
  if (Commander.check_argument("-state"))
  {
    Settings.plotState();
  }
}
