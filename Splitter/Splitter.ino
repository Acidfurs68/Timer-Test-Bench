#include "I2Cdev.h"

volatile bool commandReceived = false;
String receivedText = "";  

void setup() {
    Wire.begin(0x09); 
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    Serial.begin(9600);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
}

void loop() {
    if (commandReceived) {
        Serial.println("Command 0xBB received, toggling LED.");
        digitalWrite(12, HIGH);
        delay(6000);
        digitalWrite(12, LOW);
        commandReceived = false;
    }
    receivedText = "";
}

void receiveEvent(int howMany) {
    while (Wire.available()) {
        byte receivedByte = Wire.read();
        
        if (receivedByte == 0xBB) {
            commandReceived = true;
            receivedText = "";  
            break;  
        }

        receivedText += (char)receivedByte;

        if (receivedText == "Ping") {
            Serial.println("Ping received");
            receivedText = "";  
        }
    }
}

void requestEvent() {
    digitalWrite(13, HIGH);   
    delay(100);
    digitalWrite(13, LOW);
    Wire.write("Pong");
}
