
#include "Adafruit_VL53L1X.h"
#include "MPU9250.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <Arduino.h>
#include "sensorData.h"
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
MPU9250 mpu;
SensorData sensor_data;
static int DistanceSensorDelay =  250;

void print_roll_pitch_yaw()
{
  Serial.print("Yaw, Pitch, Roll: ");
  Serial.print(mpu.getYaw(), 2);
  Serial.print(", ");
  Serial.print(mpu.getPitch(), 2);
  Serial.print(", ");
  Serial.println(mpu.getRoll(), 2);
}

void print_calibration() {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu.getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());
    Serial.println();
    Serial.println("mag scale []: ");
    Serial.print(mpu.getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());
    Serial.println();
}
void printSensorData() {
      Serial.println("accel x: ");
      Serial.println(mpu.getAccX());
      Serial.println(mpu.getLinearAccX());

      Serial.println("accel y: ");
      Serial.println(mpu.getAccY());
      Serial.println(mpu.getLinearAccY());
      Serial.println("accel z: ");
      Serial.println(mpu.getAccZ());
      Serial.println(mpu.getLinearAccZ());
}

// Helper function to check if sensor is connected
bool isVL53L1XSensorConnected(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0); // Returns true if sensor is responsive
}

// Helper function to initialize a sensor with a unique address
bool initializeVL53L1XSensor(Adafruit_VL53L1X* sensor, int xshut_pin, int i2c_address) {
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
void disableAllVL53L1XSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], LOW);
    }
}

// Helper function to enable all sensors
void enableAllVL53L1XSensors() {
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
        digitalWrite(distance_sensors_xshut_pins[i], HIGH);
    }
}

// Helper function to print sensor data
void printVL53L1XSensorsData(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true) {
    for (int i = 0; i < distance_sensors.size(); i++) {
      if(!isVL53L1XSensorConnected(distance_sensors[i].second)) {
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
    vTaskDelay(delay_in_ms / portTICK_PERIOD_MS);
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
            print_roll_pitch_yaw();
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

  // calibrate anytime you want to
  Serial.println("Accel Gyro calibration will start in 5sec.");
  Serial.println("Please leave the device still on the flat plane.");
  mpu.verbose(true);
  delay(5000);
  mpu.calibrateAccelGyro();

  Serial.println("Mag calibration will start in 5sec.");
  Serial.println("Please Wave device in a figure eight until done.");
  delay(5000);
  mpu.calibrateMag();
  Serial.println("done calibrating");

  print_calibration();
  mpu.verbose(true);
  mpu.setMagneticDeclination(5.14);
  mpu.setFilterIterations(10);
  mpu.selectFilter(QuatFilterSel::MADGWICK);
  
  xTaskCreate(printVL53L1XSensorsData, "printVL53L1XSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
  //xTaskCreate(printMPUSensorData, "printMPUSensorsData", STACK_SIZE, &DistanceSensorDelay, 1, nullptr);
}

void loop() {
  if (mpu.update())
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
  }
}
