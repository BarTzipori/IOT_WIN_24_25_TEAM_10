
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

// Helper function to check if sensor is connected
bool isSensorConnected(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0); // Returns true if sensor is responsive
}

// Helper function to enable only one sensor
bool enableOnlyOneSensor(size_t index) {
    // Disable all sensors first
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], LOW); // Pull XSHUT LOW to disable
    }
    // Enable the desired sensor
    digitalWrite(distance_sensors_xshut_pins[index], HIGH); // Pull XSHUT HIGH to enable
    delay(10); // Allow stabilization
    if(!isSensorConnected(0x29)) {
        Serial.print("Sensor: ");
        Serial.print(index);
        Serial.println(" not connected");
        return false;
    } else {
        Serial.print("Sensor: ");
        Serial.print(index);
        Serial.println(" connected, initializing."); 
        // Reinitialize the sensor
        if (!distance_sensors[index]->begin(0x29, &Wire)) {
            Serial.printf("Error reinitializing sensor %d\n", index);
        }
        // Start ranging again
        if (!distance_sensors[index]->startRanging()) {
            Serial.printf("Error starting ranging on sensor %d\n", index);
        }
        distance_sensors[index]->setTimingBudget(50); //Set timing budget
        return true;
    }
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

  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */
}

void loop() {
  int16_t distance;
  for (int i = 0; i < distance_sensors.size(); i++) {
    if(enableOnlyOneSensor(i)) { // Enable only one sensor
      delay(100);
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
    } else {
      distance = -1;
      Serial.println(F("could not measure distance since the sensor is disconnected"));
    }
  }
  Serial.println();
  delay(2000);
}
