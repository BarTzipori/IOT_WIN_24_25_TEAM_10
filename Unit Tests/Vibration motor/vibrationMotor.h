#ifndef VIBRATIONMOTOR_H
#define VIBRATIONMOTOR_H

#include "Arduino.h"

// Enum for vibration patterns
enum class vibrationPattern {
  shortBuzz,
  longBuzz,
  doubleBuzz,
  pulseBuzz
};

class vibrationMotor {
    private:
        int motorPin;
    public:
        vibrationMotor(int pin);
        ~vibrationMotor() = default;
        void activate(int duration);
        void vibrate(vibrationPattern pattern);
};
#endif