
#include <Arduino.h>
#include <vector>
#include <utility>
#include "Adafruit_VL53L1X.h"
#include "MPU9250.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sensorData.h"
#include "sensorHelperFunctions.h"

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5
#define STACK_SIZE 2048

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2};
std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, 0x60},  {&vl53_2, 0x61}};
MPU9250 mpu;
SensorData sensor_data;
static int DistanceSensorDelay = 250;
bool calibration_needed = false;
bool system_calibrated = false;
TwoWire secondBus = TwoWire(1);

//Samples VL53L1X sensors data
void sampleVL53L1XSensorsData(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true) {
    for (int i = 0; i < distance_sensors.size(); i++) {
      if(!isVL53L1XSensorConnected(distance_sensors[i].second, &secondBus)) {
          Serial.print("Sensor: ");
          Serial.print(i+1);
          Serial.println(" not connected");
          continue;
      } else {
          vTaskDelay(delay_in_ms);
          if(distance_sensors[i].first->dataReady()) {
              int distance = distance_sensors[i].first->distance();
              if (distance == -1) {
                //Serial.print(F("Couldn't get distance: "));
                Serial.println(distance_sensors[i].first->vl_status);
                if(i == 0) {
                  sensor_data.setSensor1Distance(distance);
                } else {
                  sensor_data.setSensor2Distance(distance);
                }
                continue;
              }
              if(i == 0) {
                sensor_data.setSensor1Distance(distance);
              } else {
                sensor_data.setSensor2Distance(distance);
              }
              distance_sensors[i].first->clearInterrupt();
          } else {
            Serial.println(F("Data not ready"));
          }   
      }
    }
    vTaskDelay(delay_in_ms);
  }
}
//samples MPU sensor data
void sampleMPUSensorData(void *pvParameters) {
    int delay_in_ms = *(int *)pvParameters;
    while (true) {
        // Update MPU data
        if (mpu.update()) {
            // Print roll, pitch, and yaw every 25 ms
          printMPURollPitchYaw(&mpu);
          sensor_data.setPitch(mpu.getPitch());
          int yaw = mpu.getYaw();
          if(yaw < 0 ) {
            yaw = 360 + yaw;
          }
          sensor_data.setYaw(yaw);
          sensor_data.setRoll(mpu.getRoll());
          sensor_data.setAccelX(mpu.getLinearAccX());
          sensor_data.setAccelY(mpu.getLinearAccY());
          sensor_data.setAccelZ(mpu.getLinearAccZ());
          sensor_data.setlastUpdateTime(millis());
        }
        // Wait for the next cycle
        vTaskDelay(delay_in_ms);
    }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin(17,18);
  Wire.setClock(400000); // Set I2C clock speed to 100 kHz
  secondBus.begin(15,16);
  secondBus.setClock(400000); // Set I2C clock speed to 100 kHz

  while (!Serial) delay(10);

  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }
  // Initialize Distance measuring sensors
  initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second, &secondBus);
  initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &secondBus);  

  //Initializes MPU
  if (!mpu.setup(0x68)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
          delay(5000);
      }
  }
  calibrateMPU(mpu, calibration_needed);
  delay(15000);
  system_calibrated = true;
  
  //Creates threaded tasks
  //xTaskCreate(sampleVL53L1XSensorsData, "sampleVL53L1XSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
  xTaskCreate(sampleMPUSensorData, "sampleMPUSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
}

void loop() {
  delay(100);
  if(system_calibrated) {
    printMPURollPitchYaw(&mpu);
    sensor_data.printData();
  }
}
