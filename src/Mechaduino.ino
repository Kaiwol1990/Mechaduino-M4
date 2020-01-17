/*
  Mechaduino Firmware  --multi file branch

  SAM21D51 (Adafruit Metro M4 compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.


  Controlled via a Serial terminal at 250000 baud.
  ...send help to get all commands

*/

#include "core/boot.h"

//#include "modules/Cmd.h"
#include "core/State.h"

// -------------------------------------
// -               setup               -
// -------------------------------------
void setup()
{
  boot();
}

// -------------------------------------
// -               loop                -
// -------------------------------------
void loop()
{
  myCommander.cmdPoll();
}
