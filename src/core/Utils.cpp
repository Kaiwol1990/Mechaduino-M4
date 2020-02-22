

#include "SAMD51/board.h"
//#include "SAMD51/samd51TC4.h"

#include "core/Utils.h"
#include "core/objects.h"

void enaInterrupt()
{
  if (Settings.currentSettings.use_enable)
  {
    if (digitalRead(ena_pin))
    { // check if ena_pin is HIGH
      PID.disable();
    }
    else
    {
      //PID.enable();
      PID.disable();
    }
  }
}
