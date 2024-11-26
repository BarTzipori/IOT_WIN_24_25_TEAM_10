#include "vibrationMotor.h"

#define MOTOR_1_PIN 16
#define MOTOR_2_PIN 40

//Purpose:
//This is a small unit test to test the vibrationMotor class. It will test the shortBuzz and longBuzz patterns.
//This test aims to ensure that the vibrationMotor class is functioning correctly, and that the vibration patterns are working as expected.
//The patterns for the vibration motor are defined in the vibrationPattern enum in the vibrationMotor.h file.
vibrationMotor motor1(MOTOR_1_PIN); // Motor 1 on pin 16
vibrationMotor motor2(MOTOR_2_PIN); // Motor 2 on pin 25

void setup() {

}

void loop() {
  // Test Motor 1. cycle through all current vibration patterns.
  motor1.vibrate(vibrationPattern::shortBuzz);
  motor2.vibrate(vibrationPattern::shortBuzz);
  delay(2000); // Wait 2 seconds
  motor1.vibrate(vibrationPattern::longBuzz);
  motor2.vibrate(vibrationPattern::longBuzz);
  delay(2000);
  motor1.vibrate(vibrationPattern::doubleBuzz);
  motor2.vibrate(vibrationPattern::doubleBuzz);
  delay(2000);
  motor1.vibrate(vibrationPattern::pulseBuzz);
  motor2.vibrate(vibrationPattern::pulseBuzz);
  delay(2000);
}