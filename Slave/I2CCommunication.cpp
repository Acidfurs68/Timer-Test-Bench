// I2CCommunication.cpp
#include "I2CCommunication.h"
#include <Arduino.h>


void sendI2CCommand(byte command) {
  Wire.beginTransmission(0x08);
  Wire.write(command);
  Wire.endTransmission();

  Wire.beginTransmission(0x09);
  Wire.write(command);
  Wire.endTransmission();
}

