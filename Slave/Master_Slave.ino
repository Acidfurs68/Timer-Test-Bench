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
    Wire.beginTransmission(0x09); 
    Wire.write("Ping"); 
    Wire.endTransmission(); 
    digitalWrite(LED_PIN, LOW);

    delay(500); 

    Wire.requestFrom(0x09, 4); 
    bool responseReceived = false;
    while(Wire.available()) {
        char c = Wire.read(); 
        Serial.print(c); 
        responseReceived = true;
    }
    if (responseReceived) {
        lastResponseTime = millis(); 
    }
    Serial.println();
    delay(500); 

    if (millis() - lastResponseTime > responseTimeout) {

        for (int i = 0; i < RELAY_COUNT; i++) {
            digitalWrite(relayPins[i], HIGH);
        }
        currentRelay = RELAY_COUNT; 
    }
        
    if (digitalRead(pinSensor) == HIGH) {
        for (int i = 0; i < RELAY_COUNT; i++) {
            digitalWrite(relayPins[i], HIGH); 
            delay(relayDelay);              
            digitalWrite(relayPins[i], LOW);  
        }
    } else {
        resetRelays();
    }

    // Gestion du buzzer
    if (digitalRead(pinSensor) == HIGH) {
        tone(pinBuzzer, 1000);  
        delay(100);             
        noTone(pinBuzzer);      
        delay(100);             
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
        if (currentRelay >= RELAY_COUNT) {
            currentRelay = 0;
        }
    }
}


void processResponseFromSlave2() {
    lastResponseTime = millis();
}

void resetRelays() {
    for (int i = 0; i < RELAY_COUNT; i++) {
        digitalWrite(relayPins[i], LOW);
    }
    currentRelay = 0;
}
