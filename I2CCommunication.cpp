// I2CCommunication.cpp
#include "I2CCommunication.h"
#include <Arduino.h>


void sendI2CCommand(byte command) {
  Wire.beginTransmission(8);  // Adresse I2C de l'esclave
  Wire.write(command);
  Wire.endTransmission();
}
