//Contains the definitions of the functions used by the firmware.

#include "core/monitoring.h"
#include "core/objects.h"

/******************** CONSTRUCTOR *******************/
Monitoring_class::Monitoring_class()
{
}

/******************** Public *******************/
void Monitoring_class::check()
{
  Monitoring_class::ERROR.bit.UNDERVOLTAGE = mysamd51ADCSM.getV() < 10.0;
  Monitoring_class::ERROR.bit.COILOVERTEMP = mysamd51ADCSM.getT() > 50.0;
  Monitoring_class::ERROR.bit.COILUNDERTEMP = mysamd51ADCSM.getT() < -15.0;
}

void Monitoring_class::printError()
{
  Serial.print("Content of error register: ");
  Serial.println(Monitoring_class::ERROR.reg, BIN);

  if (Monitoring_class::ERROR.bit.UNDERVOLTAGE)
  {
    Serial.println("Bus voltage below 10.0V!");
  }

  if (Monitoring_class::ERROR.bit.COILOVERTEMP)
  {
    Serial.println("Coil temperature above 45.0 °C");
  }

  if (Monitoring_class::ERROR.bit.COILUNDERTEMP)
  {
    Serial.println("Coil temperature below -15.0 °C");
  }
}

bool Monitoring_class::errorOccured()
{

  if (Monitoring_class::ERROR.reg)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/******************** PRIVAT *******************/
