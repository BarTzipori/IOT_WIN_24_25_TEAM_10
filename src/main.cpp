
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
std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, 0x30},  {&vl53_2, 0x31}};
MPU9250 mpu;
SensorData sensor_data;
static int DistanceSensorDelay =  25;
bool calibration_needed = false;
bool system_calibrated = false;

// Helper function to print sensor data
void printVL53L1XSensorsData(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true && system_calibrated) {
    for (int i = 0; i < distance_sensors.size(); i++) {
      if(!isVL53L1XSensorConnected(distance_sensors[i].second)) {
          Serial.print("Sensor: ");
          Serial.print(i+1);
          Serial.println(" not connected");
          continue;
      } else {
          if(distance_sensors[i].first->dataReady()) {
              int distance = distance_sensors[i].first->distance();
              if (distance == -1) {
                Serial.print(F("Couldn't get distance: "));
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

void printMPUSensorData(void *pvParameters) {
    // Define the delay period in ticks (25 ms)
    const TickType_t period = pdMS_TO_TICKS(250);
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (true) {
        // Update MPU data
        if (mpu.update()) {
            // Print roll, pitch, and yaw every 25 ms
            printMPURollPitchYaw(&mpu);
        }
        // Wait for the next cycle (250 ms period)
        vTaskDelayUntil(&lastWakeTime, period);
    }
}

void setup() {
  Serial.begin(115200);
  Wire.setClock(100000); // Set I2C clock speed to 100 kHz
  Wire.begin(18,17);

  while (!Serial) delay(10);

  // calibrate anytime you want to
  //#if defined(ESP_PLATFORM) || defined(ESP8266)
  //    EEPROM.begin(0x80);
  //#endif

  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }
  initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second);
  initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second);  

  if (!mpu.setup(0x68)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
          delay(5000);
      }
  }

  calibrateMPU(&mpu, calibration_needed);
  delay(1000);
  system_calibrated = true;
  xTaskCreate(printVL53L1XSensorsData, "printVL53L1XSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
  //xTaskCreate(printMPUSensorData, "printMPUSensorsData", STACK_SIZE, &DistanceSensorDelay, 1, nullptr);
}

void loop() {
  delay(25);
  sensor_data.printData();
  /*if (mpu.update())
  {
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 25) {

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
      sensor_data.printData();
      prev_ms = millis();
    }
  }*/
}
