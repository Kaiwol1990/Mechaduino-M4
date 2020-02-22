

#include <Arduino.h>

#ifndef _AS5047D_class_H_
#define _AS5047D_class_H_

/******************************************* CLASS ************************************************/
class AS5047D_class
{
  //ERRFL (0x0001)
  typedef union ERRFL {
    struct
    {
      uint16_t FRERR : 1;   /* bit:      0 Framing error: is set to 1 when a non-compliant SPI frame is detected  */
      uint16_t INVCOMM : 1; /* bit:      1 Invalid command error: set to 1 by reading or writing an invalid register address  */
      uint16_t PARERR : 1;  /* bit:      2 Parity error   */
      uint16_t : 13;        /* bit: 3.. 15 reserved   */
    } bit;                  /* Struct used for bit acces   */
    uint16_t reg;           /* Type used for register acces */
  } ERRFL_Type;

  // DIAAGC (0x3FFC)
  typedef union DIAAGC {
    struct
    {
      uint16_t AGC : 8;  /* bit:   0.. 7 Automatic gain control value   */
      uint16_t LF : 1;   /* bit:       8 Offset compensation, LF=0:internal offset loops not ready regulated, LF=1:internal offset loop finished  */
      uint16_t COF : 1;  /* bit:       9 CORDIC overflow  */
      uint16_t MAGH : 1; /* bit:      10 Magnetic field strength too high; AGC=0x00 */
      uint16_t MAGL : 1; /* bit:      11 Magnetic field strength too low; AGC=0xFF */
      uint16_t : 4;      /* bit: 12.. 15 Reserved  */
    } bit;               /* Struct used for bit acces   */
    uint16_t reg;        /* Type used for register acces */
  } DIAAGC_Type;

public:
  // public functions
  AS5047D_class(Stream *, EPortType port_, uint32_t pin_);

  // setup function
  void init(int16_t *fullsteps);

  // read angle of magnet in digits
  uint16_t readDigits();

  // read position of magnet and turn it into degrees with the calibrated lookup table
  void readAngle();
  __attribute__((always_inline));
  float getAngle();
  __attribute__((always_inline));

  // measure the noise of the sensor
  float measureNoise();

  // read dignostics
  void readDIAAGC();
  void readERRFL();

  // calculates the lookuptable with converstion from digit to deg
  void initTable(int16_t *fullsteps);

private:
  EPortType port;
  uint32_t pin;

  // private valriables
  uint16_t steps_per_revolution;
  float currentAngle;
  float lastAngle;
  float lookuptable[16384] = {};

  // private functions
  inline uint16_t getParity(uint16_t payload, uint8_t bitcount);
  //inline bool getParity(uint16_t payload);
  inline uint16_t readRegister(uint16_t regAdress);
  inline uint16_t writeRegister(uint16_t regAdress, uint32_t content);
};

#endif
