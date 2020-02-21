

#include "SAMD51/board.h"
//#include "SAMD51/samd51TC4.h"

#include "core/Utils.h"
#include "core/objects.h"

void enaInterrupt()
{
  if (mySettings.currentSettings.use_enable)
  {
    if (digitalRead(ena_pin))
    { // check if ena_pin is HIGH
      myPID.disable();
    }
    else
    {
      //myPID.enable();
      myPID.disable();
    }
  }
}
