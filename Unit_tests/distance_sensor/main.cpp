
#include "Adafruit_VL53L1X.h"
#include <vector>
#include <utility>

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5
#define STACK_SIZE 2048

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2};

std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, 0x30},  {&vl53_2, 0x31}};

// Helper function to check if sensor is connected
bool isSensorConnected(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0); // Returns true if sensor is responsive
}
.,
// Helper function to initialize a sensor with a unique address
bool initializeSensor(Adafruit_VL53L1X* sensor, int xshut_pin, int i2c_address) {
    // Enable the desired sensor
    digitalWrite(xshut_pin, HIGH); // Pull XSHUT HIGH to enable
    delay(10); // Allow stabilization
    Serial.print("Sensor: ");
    Serial.print(xshut_pin);
    Serial.println(" connected, initializing."); 
    // initialize the sensor
    if (!sensor->begin(i2c_address, &Wire)) {
        Serial.printf("Error reinitializing sensor %d\n", index);
        return false;
    }
    // Start ranging 
    if (!sensor->startRanging()) {
        Serial.printf("Error starting ranging on sensor %d\n", index);
        return false;
    }
    sensor->setTimingBudget(50); //Set timing budget
    return true;
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

// Helper function to print sensor data
void printSensorsData(int delay_in_ms) {
  for (int i = 0; i < distance_sensors.size(); i++) {
    delay(100);
    if(!isSensorConnected(distance_sensors[i].second)) {
        Serial.print("Sensor: ");
        Serial.print(i);
        Serial.println(" not connected");
        continue;
    } else {
        if(distance_sensors[i].first->dataReady()) {
            int distance = distance_sensors[i].first->distance();
            if (distance == -1) {
              Serial.print(F("Couldn't get distance: "));
              Serial.println(distance_sensors[i].first->vl_status);
              continue;
            } 
            Serial.print(F("Distance: "));
            Serial.print(distance);
            Serial.print(" mm from sensor: ");
            Serial.println(i);
            distance_sensors[i].first->clearInterrupt();
        } else {
          Serial.println(F("Data not ready"));
        }
    }
  }
  Serial.println();
  delay(delay_in_ms);
}



void setup() {
  Serial.begin(115200);
  Wire.begin(18,17);
  while (!Serial) delay(10);
  xTaskCreate()
  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }

  initializeSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second);
  initializeSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second);  
}

void loop() {
  printSensorsData(1000);
}
