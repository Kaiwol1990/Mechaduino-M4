/***
   IIR Filter Library - Implementation

   Copyright (C) 2016  Martin Vincent Bloedorn

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3, as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "filters.h"

// CONSTRUCTOR AND DESTRUCTOR * * * * * * * * * * * * * * *

Filter::Filter(float hz_, float ts_) : ts(ts_),
                                       hz(hz_)
{
  init();
}

Filter::~Filter() {}

// PUBLIC METHODS * * * * * * * * * * * * * * * * * * * * *

void Filter::init(bool doFlush)
{
  if (doFlush)
    flush();

  initLowPass();
}

float_t Filter::filterIn(float input)
{

  return computeLowPass(input);
}

void Filter::flush()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    x[i] = 0.0;
    y[i] = 0.0;
  }
}

void Filter::dumpParams()
{
  uint8_t p = 6;
  Serial.println("Filter parameters:");
  Serial.print("ts\t= ");
  Serial.println(ts, p);
  Serial.print("hz\t= ");
  Serial.println(hz, p);

  Serial.print("a0\t= ");
  Serial.println(a0, p);
  Serial.print("a1\t= ");
  Serial.println(a1, p);
  Serial.print("a2\t= ");
  Serial.println(a2, p);
  Serial.print("b0\t= ");
  Serial.println(b0, p);
  Serial.print("b1\t= ");
  Serial.println(b1, p);
  Serial.print("b2\t= ");
  Serial.println(b2, p);
}

// PRIVATE METHODS  * * * * * * * * * * * * * * * * * * * *

inline float_t Filter::computeLowPass(float input)
{
  for (uint8_t i = 2; i > 0; i--)
  {
    y[i] = y[i - 1];
    x[i] = x[i - 1];
  }
  x[0] = input;

  y[0] = (b0 * x[0]) + (b1 * x[1]) + (b2 * x[2]) + (a1 * y[1]) + (a2 * y[2]);

  return y[0];
}

inline void Filter::initLowPass()
{
  // Normalisieren der Cutoff-Frequenz
  FCutoff = hz / (ts / 1.0);

  // Filter Coeffizienten berechnen
  const float ita = 1.0 / tan(PI * FCutoff);
  const float q = sqrt(2.0);
  b0 = 1.0 / (1.0 + (q * ita) + (ita * ita));
  b1 = 2 * b0;
  b2 = b0;

  a0 = 1;
  a1 = 2.0 * ((ita * ita) - 1.0) * b0;
  a2 = -(1.0 - (q * ita) + (ita * ita)) * b0;
}
