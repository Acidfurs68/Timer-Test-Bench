// I2CCommunication.h
#ifndef I2C_COMMUNICATION_H
#define I2C_COMMUNICATION_H

#include <Wire.h>
#include <Arduino.h>

void sendI2CCommand(byte command);
void checkConnection();

#endif // I2C_COMMUNICATION_H
