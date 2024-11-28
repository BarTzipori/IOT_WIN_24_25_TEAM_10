
#include "Adafruit_VL53L1X.h"
#include <vector>
#include <utility>

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5


Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2};

std::vector<Adafruit_VL53L1X*> distance_sensors = {&vl53_1, &vl53_2};

// Helper function to enable only one sensor
void enableOnlyOneSensor(size_t index) {
    // Disable all sensors first
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], LOW); // Pull XSHUT LOW to disable
    }
    // Enable the desired sensor
    digitalWrite(distance_sensors_xshut_pins[index], HIGH); // Pull XSHUT HIGH to enable
    delay(100); // Allow stabilization

    // Reinitialize the sensor
    if (!distance_sensors[index]->begin(0x29, &Wire)) {
        Serial.printf("Error reinitializing sensor %d\n", index);
        while (1) delay(100); // Halt on failure
    }
    // Start ranging again
    if (!distance_sensors[index]->startRanging()) {
        Serial.printf("Error starting ranging on sensor %d\n", index);
        while (1) delay(100);
    }

    distance_sensors[index]->setTimingBudget(50); // Set timing budget
}

// Helper function to disable all sensors
void disableAllSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], LOW);
    }
}

// Helper function to enable all sensors
void enableAllSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], HIGH);
    }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(18,17);
  while (!Serial) delay(10);

  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
      //digitalWrite(xshutPins[i], LOW); // Disable all sensors
  }
  delay(10);

  //initialize all sensors, and check if they are working (each sensor gets his own address)
  Serial.println(F("Adafruit VL53L1X multiple sensors unit test"));
  for(int i  = 0; i < distance_sensors.size(); i++) {
    if (!distance_sensors[i]->begin(0x29, &Wire)) {
      Serial.print("Error on init of sensor:");
      Serial.print(i);
      Serial.println(distance_sensors[i]->vl_status);
      delay(10);
    } else {
        Serial.println(F("VL53L1X sensor OK!"));
        Serial.print(F("Sensor ID: 0x"));
        Serial.println(distance_sensors[i]->sensorID(), HEX);
    }
  }
  //start ranging for all sensors
  for(int i  = 0; i < distance_sensors.size(); i++) {
    if (!distance_sensors[i]->startRanging()) {
      Serial.print("Error on start ranging of sensor:");
      Serial.print(i);
      Serial.println(distance_sensors[i]->vl_status);
    } else {
        Serial.println(F("Ranging started"));
          distance_sensors[i]->setTimingBudget(50);
          Serial.print(F("Timing budget (ms): "));
          // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
          Serial.println(distance_sensors[i]->getTimingBudget());
    }
    delay(100);
  }
  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */
}

void loop() {
  int16_t distance;
  for (int i = 0; i < distance_sensors.size(); i++) {
    enableOnlyOneSensor(i); // Enable only one sensor
    delay(100); // Stabilize
    if(distance_sensors[i]->dataReady()) {
      distance = distance_sensors[i]->distance();
      if (distance == -1) {
        Serial.print(F("Couldn't get distance: "));
        Serial.println(distance_sensors[i]->vl_status);
        return;
      } 
      Serial.print(F("Distance: "));
      Serial.print(distance);
      Serial.print(" mm from sensor: ");
      Serial.println(i);
      distance_sensors[i]->clearInterrupt();
    } else {
      Serial.println(F("Data not ready"));
    }
    delay(200);
  }
  Serial.println();
  delay(2000);
}
