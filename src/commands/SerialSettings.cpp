
#include "SerialMovement.h"
#include "core/State.h"
#include "modules/Cmd.h"

#include "language/en.h"

void setting_menu(int arg_cnt, char **args)
{
  Serial.println(" ");
  Serial.println("Settings menu");
  Serial.println("-------------");

  // Load settings from specific slot to the active uses settings. If no slot is given the default slot will be used
  if (check_argument(args, arg_cnt, "-load"))
  {
    int8_t slot_load = return_integer_argument(args, arg_cnt, "-load", mySettings.getDefaultSlot(), mySettings.getDefaultSlot(), 4);
    mySettings.loadSettings(slot_load);
  }

  // store current settings to specific slot. If no slot is given the default slot will be used
  if (check_argument(args, arg_cnt, "-store"))
  {
    int8_t slot_store = return_integer_argument(args, arg_cnt, "-store", mySettings.getDefaultSlot(), mySettings.getDefaultSlot(), 4);
    mySettings.storeSettings(slot_store);
  }

  // set default slot to given slot
  int8_t slot_default = return_integer_argument(args, arg_cnt, "-setdefault", -1, -1, 4);
  if (slot_default > -1)
  {
    mySettings.setDefaultSlot(slot_default);
  }

  // print content of specific slot
  if (check_argument(args, arg_cnt, "-show"))
  {
    int8_t slot_print = return_integer_argument(args, arg_cnt, "-show", -1, -1, 4);
    mySettings.plotSettings(slot_print);
  }

  // restore current settings to default settings
  if (check_argument(args, arg_cnt, "-restore"))
  {
    mySettings.loadDefaultSettings();
  }

  // save current settings (all slots) to flash
  if (check_argument(args, arg_cnt, "-save"))
  {
    mySettings.commit();
  }

  // show state of the different slots
  if (check_argument(args, arg_cnt, "-state"))
  {
    mySettings.plotState();
  }
}
