/*
  Mechaduino Firmware  --multi file branch

  SAM21D51 (Adafruit Metro M4 compatible), AS5047 encoder, A4954_class driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.


  Controlled via a Serial terminal at 250000 baud.
  ...send help to get all commands

*/

#include "core/objects.h"

// -------------------------------------
// -               loop                -
// -------------------------------------
void loop()
{

  // nothing to do here
  // all commands are handled via TC interrupts
  Commander.cmdPoll();
}
