

#include "AS5047D.h"
#include <SPI.h>

#include "SAMD51/wiringMechaduino.h"
#include "core/State.h"
#include "modules/custommath.h"

static Stream *stream;

// CONSTRUCTOR AND DESTRUCTOR * * * * * * * * * * * * * * *
AS5047D::AS5047D(Stream *str, EPortType port_, uint32_t pin_) : port(port_),
                                                                pin(pin_)
{
  stream = str;
}

// PUBLIC METHODS * * * * * * * * * * * * * * * * * * * * *
void AS5047D::init(int16_t *fullsteps)
{

  // set CS as OUTPUT
  digitalFASTpinMode(port, pin, OUTPUT);

  // write CS HIGH
  digitalFASTWrite(port, pin, HIGH);

  // SPI settings for the AS5047D
  //AS5047D SPI uses mode=1 (CPOL=0, CPHA=1)
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE1);

  // begin SPI bus
  SPI.begin();

  // wait for bus to become ready
  delay(200);

  // begin spi transaction to chip
  SPI.beginTransaction(settingsA);

  // lookuptable calculation
  initTable(fullsteps);
}

uint16_t AS5047D::readDigits()
{
  uint16_t data;

  // read register that contains angle information
  data = readRegister(0x3FFF);

  // Flush data of SPI
  data = readRegister(0x0000);

  return data;
}

void AS5047D::readAngle()
{
  uint16_t data;
  float angle;
  float raw;
  float raw_diff;
  static float lastRawAngle;

  // read register that contains angle information
  data = readRegister(0x3FFF);

  // transform 14 bit digits to angle from 0 to 360.0 deg
  // get angle from lookuptable
  raw = lookuptable[data];

  // calculate and wrap new angle
  raw_diff = raw - lastRawAngle;
  lastRawAngle = raw;

  if (raw_diff < -180.0)
  {
    angle = currentAngle + 360.0 + raw_diff;
  }
  else if (raw_diff > 180.0)
  {
    angle = currentAngle - 360.0 + raw_diff;
  }
  else
  {
    angle = currentAngle + raw_diff;
  }

  // buffer last reading for speed calculation
  lastAngle = currentAngle;

  // write new (current) angle
  currentAngle = angle;
}

float AS5047D::getAngle()
{
  return currentAngle;
}

float AS5047D::getLastAngle()
{
  return lastAngle;
}

void AS5047D::readDIAAGC()
{
  uint16_t registerValue;

  stream->println("Leese Encoder Diganostics");
  registerValue = readRegister(0x3FFC);
  registerValue = readRegister(0x0000);

  // Slice single bits from register Value
  bool MAGL = bitRead(registerValue, 11);             //  11 Diagnostics: Magnetic field strength too low; AGC=0xFF
  bool MAGH = bitRead(registerValue, 10);             //  10 Diagnostics: Magnetic field strength too high; AGC=0x00
  bool COF = bitRead(registerValue, 9);               //   9 Diagnostics: CORDIC overflow
  bool LF = bitRead(registerValue, 8);                //   8 Diagnostics: Offset compensation, LF=0:internal offset loops not ready regulated, LF=1:internal offset loop finished
  uint16_t AGC = registerValue && 0B0000000011111111; // 7:0 Automatic gain control value

  stream->println("Check DIAAGC register (0x3FFC)");
  stream->print("MAGL: ");
  stream->print(MAGL);
  if (MAGL)
  {
    stream->println(": Magnetic field strength too low!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("MAGH: ");
  stream->print(MAGH);
  if (MAGH)
  {
    stream->println(": Magnetic field strength too high!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("COF: ");
  stream->print(COF);
  if (COF)
  {
    stream->println(": CORDIC overflow!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("LF: ");
  stream->print(LF);
  if (LF)
  {
    stream->println(": Offset compensation finished!");
  }
  else
  {
    stream->println(": Offset compensation not ready!");
  }

  stream->print("AGC: Automatic gain control: ");
  stream->println(AGC);

  stream->println(" ");
}

void AS5047D::readERRFL()
{
  uint16_t registerValue;

  stream->println("Leese Encoder Diganostics");
  registerValue = readRegister(0x0001);
  registerValue = readRegister(0x0000);

  //                          0B0011111111111111
  bool PARERR = registerValue & 0B0000000000000100;  //  11 Diagnostics: Magnetic field strength too low; AGC=0xFF
  bool INVCOMM = registerValue & 0B0000000000000010; //  10 Diagnostics: Magnetic field strength too high; AGC=0x00
  bool FRERR = registerValue & 0B0000000000000001;   //   9 Diagnostics: CORDIC overflow

  stream->println("Check ERRFL register (0x0001)");
  stream->print("PARERR: ");
  stream->print(PARERR);
  if (PARERR)
  {
    stream->println(": Parity error occured!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("INVCOMM: ");
  stream->print(INVCOMM);
  if (INVCOMM)
  {
    stream->println(": Invalid command error!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("FRERR: ");
  stream->print(FRERR);
  if (FRERR)
  {
    stream->println(": Framing error occured!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->println(" ");
}

void AS5047D::initTable(int16_t *fullsteps)
{

  // calculate the ticks between the fullsteps
  float ticks[mySettings.currentSettings.steps_per_Revolution];
  int32_t counter = 0;

  int32_t low;
  int32_t high;
  int32_t iStart = 0;
  int32_t jStart = 0;
  int32_t stepNo = 0;

  for (int32_t i = 0; i < mySettings.currentSettings.steps_per_Revolution; i++)
  {
    low = i;
    high = i + 1;

    if (high >= mySettings.currentSettings.steps_per_Revolution)
    {
      high = high - mySettings.currentSettings.steps_per_Revolution;
    }

    ticks[i] = fullsteps[high] - fullsteps[low];

    if (ticks[i] < -8192)
    {
      ticks[i] += 16384;
    }
    else if (ticks[i] > 8192)
    {
      ticks[i] -= 16384;
    }

    for (int32_t j = 0; j < ticks[i]; j++)
    {
      stepNo = (mod(fullsteps[i] + j, 16384));
      if (stepNo == 0)
      {
        iStart = i;
        jStart = j;
      }
    }
  }

  float lookupAngle;
  float tick;

  int32_t p = 0;

  for (int32_t i = iStart; i < (iStart + mySettings.currentSettings.steps_per_Revolution + 1); i++)
  {

    if (i >= mySettings.currentSettings.steps_per_Revolution)
    {
      tick = ticks[i - mySettings.currentSettings.steps_per_Revolution];
    }
    else
    {
      tick = ticks[i];
    }

    if (i == iStart)
    {
      for (int32_t j = jStart; j < tick; j++)
      {
        counter += 1;

        lookupAngle = floatmod((mySettings.PA * i) + ((mySettings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }

    else if (i == (iStart + mySettings.currentSettings.steps_per_Revolution))
    {
      for (int32_t j = 0; j < jStart; j++)
      {
        counter += 1;

        lookupAngle = floatmod((mySettings.PA * i) + ((mySettings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }
    else
    {
      for (int32_t j = 0; j < tick; j++)
      {
        counter += 1;

        lookupAngle = floatmod((mySettings.PA * i) + ((mySettings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }
  }
}

// PRIVATE METHODS  * * * * * * * * * * * * * * * * * * * *

inline uint16_t AS5047D::getParity(uint16_t payload, uint8_t bitcount)
{
  uint16_t parity = 0;
  for (byte i = 0; i <= bitcount; i++)
  {
    parity += (bool)(payload & (1 << i));
  }
  parity = mod(parity, 2);

  return parity;
}

inline uint16_t AS5047D::readRegister(uint16_t regAdress)
{
  uint16_t registerValue;
  uint16_t parity;

  // cut to 14 bit
  regAdress = regAdress & 0B0011111111111111;

  // add read flag according do datasheet
  regAdress = regAdress + (1 << 14);

  // Parity berechnen
  parity = getParity(regAdress, 14);

  // add parity bit
  regAdress = regAdress + (parity << 15);

  // send and receive SPI data
  digitalFASTWrite(PORTA, 15, LOW); // write chipSelectPin LOW

  // delayNanoseconds(200);
  registerValue = SPI.transfer16(regAdress); // read register that contains angle information

  digitalFASTWrite(PORTA, 15, HIGH); // write chipSelectPin HIGH

  // check parity of received data
  parity = getParity(registerValue, 15);

  if (parity)
  {
    return 0;
  }

  return (registerValue & 0B0011111111111111);
}
