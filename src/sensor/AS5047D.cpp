

#include "sensor/AS5047D.h"
#include <SPI.h>

#include "SAMD51/wiringMechaduino.h"
#include "core/objects.h"
#include "modules/custommath.h"

static Stream *stream;

/********************************************* CONSTRUCTOR **************************************************/
AS5047D_class::AS5047D_class(Stream *str, EPortType port_, uint32_t pin_) : port(port_),
                                                                            pin(pin_)
{
  stream = str;
}

/******************************************* PUBLIC METHODS ************************************************/

// init the sensor
void AS5047D_class::init(int16_t *fullsteps)
{

  // set CS as OUTPUT
  digitalFASTpinMode(port, pin, OUTPUT);

  // write CS HIGH
  digitalFASTWrite(port, pin, HIGH);

  // SPI settings for the AS5047D_class
  //AS5047D_class SPI uses mode=1 (CPOL=0, CPHA=1)
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

// read the angle in digits without the lookup table
uint16_t AS5047D_class::readDigits()
{
  uint16_t data;

  // read register that contains angle information
  data = readRegister(0x3FFF);

  // Flush data of SPI
  data = readRegister(0x0000);

  return data;
}

// start a new angle reading
void AS5047D_class::readAngle()
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

// return the current angle of the magnet
float AS5047D_class::getAngle()
{
  return currentAngle;
}

// measure encoder noise
float AS5047D_class::measureNoise()
{
  float points[500] = {0};

  for (uint32_t i = 0; i < 500; i++)
  {
    delayMicroseconds(200);
    points[i] = AS5047D_class::readDigits();
  }

  float highest = 0;
  float lowest = 16384.0;

  for (uint32_t i = 0; i < 500; i++)
  {
    if (points[i] > highest)
    {
      highest = points[i];
    }
    else if (points[i] < lowest)
    {
      lowest = points[i];
    }
  }

  return (360.0 * abs(highest - lowest)) / 16384.0;
}

// read DIAAGC register
void AS5047D_class::readDIAAGC()
{
  // Variables
  DIAAGC_Type DIAAGC;

  // flush SPI register
  AS5047D_class::readRegister(0x3FFC);

  // read register
  DIAAGC.reg = AS5047D_class::readRegister(0x0000);

  // output
  stream->println("Check DIAAGC register (0x3FFC)");
  stream->print("Raw register value: ");
  stream->println(DIAAGC.reg, BIN);
  stream->print("MAGL: ");
  stream->print(DIAAGC.bit.MAGL);
  if (DIAAGC.bit.MAGL)
  {
    stream->println(": Magnetic field strength too low!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("MAGH: ");
  stream->print(DIAAGC.bit.MAGH);
  if (DIAAGC.bit.MAGH)
  {
    stream->println(": Magnetic field strength too high!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("COF: ");
  stream->print(DIAAGC.bit.COF);
  if (DIAAGC.bit.COF)
  {
    stream->println(": CORDIC overflow!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("LF: ");
  stream->print(DIAAGC.bit.LF);
  if (DIAAGC.bit.LF)
  {
    stream->println(": Offset compensation finished!");
  }
  else
  {
    stream->println(": Offset compensation not ready!");
  }

  stream->print("AGC: Automatic gain control: ");
  stream->println(DIAAGC.bit.AGC);
  stream->print("Guessed magnetic field strength: ");
  stream->print(70 - (35 * (float)DIAAGC.bit.AGC / 255.0));
  stream->println(" mT");

  stream->println(" ");
}

// read ERRFL register
void AS5047D_class::readERRFL()
{
  // variables
  ERRFL_Type ERRFL;

  // flush SPI
  AS5047D_class::readRegister(0x0001);

  // read register
  ERRFL.reg = readRegister(0x0000);

  // output
  stream->println("Check ERRFL register (0x0001)");
  stream->print("Raw register value: ");
  stream->println(ERRFL.reg & 0B0011111111111111, BIN);
  stream->print("PARERR: ");
  stream->print(ERRFL.bit.PARERR);
  if (ERRFL.bit.PARERR)
  {
    stream->println(": Parity error occured!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("INVCOMM: ");
  stream->print(ERRFL.bit.INVCOMM);
  if (ERRFL.bit.INVCOMM)
  {
    stream->println(": Invalid command error!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->print("FRERR: ");
  stream->print(ERRFL.bit.FRERR);
  if (ERRFL.bit.FRERR)
  {
    stream->println(": Framing error occured!");
  }
  else
  {
    stream->println(": Looks good!");
  }

  stream->println(" ");
}

// calculate the lookup table based on the fullstep readings
void AS5047D_class::initTable(int16_t *fullsteps)
{

  // calculate the ticks between the fullsteps
  float ticks[Settings.currentSettings.steps_per_Revolution];
  int32_t counter = 0;

  int32_t low;
  int32_t high;
  int32_t iStart = 0;
  int32_t jStart = 0;
  int32_t stepNo = 0;

  for (int32_t i = 0; i < Settings.currentSettings.steps_per_Revolution; i++)
  {
    low = i;
    high = i + 1;

    if (high >= Settings.currentSettings.steps_per_Revolution)
    {
      high = high - Settings.currentSettings.steps_per_Revolution;
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

  for (int32_t i = iStart; i < (iStart + Settings.currentSettings.steps_per_Revolution + 1); i++)
  {

    if (i >= Settings.currentSettings.steps_per_Revolution)
    {
      tick = ticks[i - Settings.currentSettings.steps_per_Revolution];
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

        lookupAngle = floatmod((Settings.PA * i) + ((Settings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }

    else if (i == (iStart + Settings.currentSettings.steps_per_Revolution))
    {
      for (int32_t j = 0; j < jStart; j++)
      {
        counter += 1;

        lookupAngle = floatmod((Settings.PA * i) + ((Settings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }
    else
    {
      for (int32_t j = 0; j < tick; j++)
      {
        counter += 1;

        lookupAngle = floatmod((Settings.PA * i) + ((Settings.PA * j) / tick), 360);

        lookuptable[p] = lookupAngle;

        p++;
      }
    }
  }
}

/******************************************* PRIVATE METHODS ************************************************/

// calculate parity bit
inline uint16_t AS5047D_class::getParity(uint16_t payload, uint8_t bitcount)
{
  uint16_t parity = 0;
  for (byte i = 0; i <= bitcount; i++)
  {
    parity += (bool)(payload & (1 << i));
  }
  parity = mod(parity, 2);

  return parity;
}

// read register from chip with given adress
inline uint16_t AS5047D_class::readRegister(uint16_t regAdress)
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
