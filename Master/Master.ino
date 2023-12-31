#include <Keypad.h>
#include <LedControl.h>
#include "I2CCommunication.h"
#include "I2Cdev.h"

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {2, 3, 4, 5};

Keypad keypadObj = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LedControl lc = LedControl(10, 11, 12, 1);

long csecondsLeft;
int decrement = 3;
bool explosed = false;
bool defused = false;

long hours = 0;
long minutes = 0;
long seconds = 0;
long hundredths = 0;

String code = "";

char keypadOutput[32] = {0};
byte playSound = A0;

bool isProgrammingTime = false;
String inputKeys = "";
String programmedTime = "";
bool isCountingDown = false;

void setup() {
  csecondsLeft = 0;
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 10);
  lc.clearDisplay(0);

for (int i = 0; i < 3; ++i) {
    tone(playSound, 2000, 80);
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(150);
}

for (int i = 0; i < 1; ++i) {
    lc.shutdown(i, false);      
    lc.setIntensity(i, 10);     
    lc.clearDisplay(i);          
  }

  createXEffect();

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(13, OUTPUT);

  analogWrite(A0, 0);

  Wire.begin(8); 
}

void createXEffect() {
    byte a_segment = B01000000; 
    byte d_segment = B00001000; 

    for (int i = 0; i < 4; ++i) {
        lc.setRow(0, i, d_segment);
        lc.setRow(0, 7 - i, a_segment);

        delay(80);
        lc.clearDisplay(0);
    }

    for (int i = 3; i >= 0; --i) {
        lc.setRow(0, i, a_segment);
        lc.setRow(0, 7 - i, d_segment);

        delay(80);
        lc.clearDisplay(0);
    }
}

void updateDisplay() {
  long remainingSeconds = csecondsLeft / 60;
  hours = remainingSeconds / 3600;
  remainingSeconds %= 3600;
  minutes = remainingSeconds / 60;
  seconds = remainingSeconds % 60;
  hundredths = csecondsLeft % 60;

  lc.setDigit(0, 7, hours / 10, false);
  lc.setDigit(0, 6, hours % 10, false);
  lc.setDigit(0, 5, minutes / 10, false);
  lc.setDigit(0, 4, minutes % 10, false);
  lc.setDigit(0, 3, seconds / 10, false);
  lc.setDigit(0, 2, seconds % 10, false);
  lc.setDigit(0, 1, hundredths / 10, false);
  lc.setDigit(0, 0, hundredths % 10, false);
}

void programTime() {
  if (inputKeys.length() >= 8) {
    inputKeys = inputKeys.substring(0, 8);
    programmedTime = inputKeys.substring(0, 2) + ":" + inputKeys.substring(2, 4) + ":" + inputKeys.substring(4, 6) + ":" + inputKeys.substring(6, 8);

    Serial.print("inputKeys: ");
    Serial.println(inputKeys);
    Serial.print("programmedTime: ");
    Serial.println(programmedTime);

    lc.clearDisplay(0);

    inputKeys = "";
  }
}

void startCountdown() {
  Serial.println("Starting countdown...");

  if (programmedTime.length() == 11) {
    hours = programmedTime.substring(0, 2).toInt();
    minutes = programmedTime.substring(3, 5).toInt();
    seconds = programmedTime.substring(6, 8).toInt();
    hundredths = programmedTime.substring(9, 11).toInt();

    csecondsLeft = ((hours * 3600) + (minutes * 60) + seconds) * 60 + hundredths;
  } else {
    Serial.println("Invalid programmed time. Please enter a time greater than zero.");
    return;
  }

  updateDisplay();
  isCountingDown = true;
}

void loop() {

  char key = keypadObj.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key >= '0' && key <= '9') {
      int num = key - '0';
      int displayIndex = 7 - inputKeys.length();
      lc.setDigit(0, displayIndex, num, false);
      inputKeys += key;
      tone(playSound, 2000, 50);
    }

    if (!isProgrammingTime && key == 'A') {
      Serial.println("Programming time...");
      isProgrammingTime = true;
      inputKeys = "";
      programmedTime = "";
      lc.clearDisplay(0);
    } else if (isProgrammingTime && key == 'D') {
      isProgrammingTime = false;
      programTime();
      Serial.print("Programmed Time: ");
      Serial.println(programmedTime);
      startCountdown();
      programmedTime = "";
    } else if (isCountingDown && key == 'B') {
      Serial.println(code);
      decrement = 0;
      defused = true;
      code = "";
    } else if (isCountingDown && key == 'D') {
      Serial.println(code);
      decrement = 3;
      defused = false;
      code = "";
    } else if (key == 'C') {
      isProgrammingTime = false;
      lc.setDigit(0, 7, 0, false);
      lc.clearDisplay(0);
    }
  }

  
  if (isCountingDown) {
    delay(50);
    csecondsLeft -= decrement;
    updateDisplay();

    if (csecondsLeft % 60 == 0) {
      tone(playSound, 3236, 150);
    }

    if (csecondsLeft < 0) {
      csecondsLeft = 0;
      if (!explosed) {
        digitalWrite(13, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A3, HIGH);
        digitalWrite(A6, HIGH);
        tone(playSound, 2000, 6000);
        explosed = true;
      }
    }

    if (csecondsLeft == 0) {
        sendI2CCommand(0xAA);
        sendI2CCommand(0xFF);
    }
  }
}




