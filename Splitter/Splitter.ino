#include "I2Cdev.h"

volatile bool commandReceived = false;
String receivedText = "";  // Accumuler les caractères du message textuel

void setup() {
    Wire.begin(0x09); // Rejoint le bus I2C avec l'adresse #9
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
    // Réinitialiser le texte reçu si nécessaire
    receivedText = "";
}

void receiveEvent(int howMany) {
    while (Wire.available()) {
        byte receivedByte = Wire.read();
        
        // Vérifier si c'est une commande
        if (receivedByte == 0xBB) {
            commandReceived = true;
            receivedText = "";  // Réinitialiser le texte reçu
            break;  // Sortir de la boucle
        }

        // Accumuler le texte si ce n'est pas une commande
        receivedText += (char)receivedByte;

        // Traiter le message textuel si nécessaire (par exemple, "Ping")
        if (receivedText == "Ping") {
            Serial.println("Ping received");
            receivedText = "";  // Réinitialiser pour le prochain message
        }
    }
}

void requestEvent() {
    digitalWrite(13, HIGH);   // Allume la LED de test
    delay(100);
    digitalWrite(13, LOW);
    Wire.write("Pong");
}
