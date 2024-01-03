#include "I2Cdev.h"
#include "I2CCommunication.h"

#define LED_PIN 13
#define RELAY_COUNT 8
int relayPins[RELAY_COUNT] = {A0, A1, A2, A3, A4, A5, A6, A7};

int currentRelay = 0;
unsigned long previousMillis = 0;
const unsigned long relayDelay = 50;  

unsigned long lastResponseTime = 0;
const unsigned long responseTimeout = 1000;

bool received0xFF = false; 

const int pinBuzzer = A15;
const int pinSensor = A8;

void setup() {
  Serial.begin(9600);
  Wire.begin(0x08);
  pinMode(LED_PIN, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinSensor, INPUT_PULLUP);
  Wire.onReceive(receiveEvent);
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

}

void loop() {
    if (received0xFF) { 
        sendI2CCommand(0xBB);
        received0xFF = false;  
    }

    digitalWrite(LED_PIN, HIGH);
    Wire.beginTransmission(0x09); // Commence la transmission à l'adresse de l'esclave
    Wire.write("Ping"); // Envoie le message
    Wire.endTransmission(); // Termine la transmission
    digitalWrite(LED_PIN, LOW);

    delay(500); // Attente pour la réponse

    Wire.requestFrom(0x09, 4); // Demande 4 caractères de l'esclave
    bool responseReceived = false;
    while(Wire.available()) {
        char c = Wire.read(); // reçoit un byte en tant que caractère
        Serial.print(c); // Affiche le caractère sur le moniteur série
        responseReceived = true;
    }
    if (responseReceived) {
        lastResponseTime = millis(); // Mise à jour du temps de la dernière réponse
    }
    Serial.println();
    delay(500); // Attente avant de renvoyer un message

    // Vérifier si la communication est perdue
    if (millis() - lastResponseTime > responseTimeout) {

        for (int i = 0; i < RELAY_COUNT; i++) {
            digitalWrite(relayPins[i], HIGH);
        }
        currentRelay = RELAY_COUNT; // S'assurer que tous les relais sont considérés comme activés
    }
        
    if (digitalRead(pinSensor) == HIGH) {
        // Le pont est retiré, activer les relais en séquence
        for (int i = 0; i < RELAY_COUNT; i++) {
            digitalWrite(relayPins[i], HIGH); // Activer le relais
            delay(relayDelay);              // Attendre
            digitalWrite(relayPins[i], LOW);  // Désactiver le relais
        }
    } else {
        // Réinitialiser l'état des relais lorsque le pont est en place
        resetRelays();
    }

    // Gestion du buzzer
    if (digitalRead(pinSensor) == HIGH) {
        tone(pinBuzzer, 1000);  // Fréquence du buzzer à 1000 Hz
        delay(100);             // Durée du son
        noTone(pinBuzzer);      // Arrêter le son
        //delay(100);             // Durée de pause entre les sons
    } else {
        noTone(pinBuzzer);
    }
}


void receiveEvent() {
  while (Wire.available()) {
    byte command = Wire.read();

    Serial.print("Received command: ");
    Serial.println(command, HEX);

    if (command == 0xAA) {
      activateRelays();  
    } else if (command == 0xFF){
      received0xFF = true;
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
        }
        // Réinitialisez currentRelay si tous les relais ont été activés
        if (currentRelay >= RELAY_COUNT) {
            currentRelay = 0;
        }
    }
}


void processResponseFromSlave2() {
    // Mettre à jour le temps de la dernière réponse
    lastResponseTime = millis();

    // ... Traitement de la réponse ...
}

void resetRelays() {
    for (int i = 0; i < RELAY_COUNT; i++) {
        digitalWrite(relayPins[i], LOW);
    }
    currentRelay = 0;
}
