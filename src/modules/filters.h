/***

 */

#pragma once

/// Has to be executed on Arduino IDE > 1.6.7
#include <Arduino.h>

class Filter
{
public:
  /** \brief Filter class.
   *
   */
  Filter(float hz_, float ts_);
  ~Filter();

  float filterIn(float input);

  void flush();
  void init(bool doFlush = true);

  void setSamplingTime(float ts_, bool doFlush = true)
  {
    ts = ts_;
    init(doFlush);
  }
  void setCutoffFreqHZ(float hz_, bool doFlush = true)
  {
    hz = hz_;
    init(doFlush);
  }

  void dumpParams();

private:
  float ts;
  float hz;

  // Helper variables during coefficient calcutions
  float ita, q, FCutoff;

  // Filter coefficients
  float b0, b1, b2, a0, a1, a2;

  // Filter buffer
  float y[3];
  float x[3];

  inline float computeLowPass(float input);

  inline void initLowPass();
};
