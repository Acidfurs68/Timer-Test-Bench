#include <Wire.h>

#define RELAY_COUNT 8
int relayPins[RELAY_COUNT] = {A0, A1, A2, A3, A4, A5, A6, A7};

int currentRelay = 0;
unsigned long previousMillis = 0;
const unsigned long relayDelay = 100;  

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }
}

void loop() {
  // Le code de l'esclave peut effectuer d'autres tâches ici si nécessaire
}

void receiveEvent() {
  while (Wire.available()) {
    byte command = Wire.read();

    Serial.print("Received command: ");
    Serial.println(command, HEX);

    if (command == 0xAA) {
      activateRelays();
    } else {
      // Gérer d'autres commandes si nécessaire
    }
  }
}

void activateRelays() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= relayDelay) {
    previousMillis = currentMillis;

    if (currentRelay < RELAY_COUNT) {
      digitalWrite(relayPins[currentRelay], HIGH);
      currentRelay++;
    } else {
      // Réinitialiser pour la prochaine séquence
      currentRelay = 0;
    }
  }
}