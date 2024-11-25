#include "vibrationMotor.h"

// Constructor to initialize the motor pin
vibrationMotor::vibrationMotor(int motorPin) : motorPin(motorPin) {
  pinMode(motorPin, OUTPUT);
}

void vibrationMotor::activate(int duration) {
    digitalWrite(motorPin, HIGH);
    delay(duration);
    digitalWrite(motorPin, LOW);
}

static void multipleVibrations(int vibrationNumber, int delayNumber, int motorPin) {
        for (int i = 0; i < vibrationNumber; i++) {
        digitalWrite(motorPin, HIGH);
        delay(delayNumber);
        digitalWrite(motorPin, LOW);
        delay(50);          
    }
}

void vibrationMotor::vibrate(vibrationPattern pattern) {
    switch (pattern) {
        case vibrationPattern::shortBuzz:
            // Short buzz
            digitalWrite(motorPin, HIGH);
            delay(400);
            digitalWrite(motorPin, LOW);
            delay(100);
            break;
        case vibrationPattern::longBuzz:
            // Long buzz
            digitalWrite(motorPin, HIGH);
            delay(800);
            digitalWrite(motorPin, LOW);
            delay(100);
            break;
        case vibrationPattern::doubleBuzz:
            // Double buzz
            multipleVibrations(5, 350, motorPin);
            break;
        case vibrationPattern::pulseBuzz:
            // Pulse buzz
            multipleVibrations(5, 250, motorPin);
    }
}