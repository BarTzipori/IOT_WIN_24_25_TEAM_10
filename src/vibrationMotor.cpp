#include "vibrationMotor.h"

// Constructor to initialize the motor pin
vibrationMotor::vibrationMotor(int motorPin) : motorPin(motorPin) {
  pinMode(motorPin, OUTPUT);
}

void vibrationMotor::vibrate(vibrationPattern pattern) {
    switch (pattern) {
        case vibrationPattern::shortBuzz:
            // Short buzz
            digitalWrite(motorPin, HIGH);
            delay(50);
            digitalWrite(motorPin, LOW);
            delay(50);
            break;
        case vibrationPattern::longBuzz:
            // Long buzz
            digitalWrite(motorPin, HIGH);
            delay(500);
            digitalWrite(motorPin, LOW);
            delay(50);
            break;
        case vibrationPattern::doubleBuzz:
            // Double buzz
            for (int i = 0; i < 2; i++) {
                digitalWrite(motorPin, HIGH);
                delay(50);
                digitalWrite(motorPin, LOW);
                delay(50);
            }
            break;
        case vibrationPattern::pulseBuzz:
            // Pulse buzz
            for (int i = 0; i < 3; i++) {
                digitalWrite(motorPin, HIGH);
                delay(50);
                digitalWrite(motorPin, LOW);
                delay(50);
            }
            break;
    }
}