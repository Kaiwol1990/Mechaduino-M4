

#include "custommath.h"

int32_t mod(int32_t xMod, int32_t mMod)
{
  return ((xMod % mMod) + mMod) % mMod;
}

float floatmod(float xMod, float mMod)
{
  return fmod((fmod(xMod, mMod) + mMod), mMod);
}
