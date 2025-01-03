
#include <Arduino.h>
#include <vector>
#include <utility>
#include <ezButton.h>
#include "Adafruit_VL53L1X.h"
#include "MPU9250.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sensorData.h"
#include "sensorHelperFunctions.h"
#include "vibrationMotor.h"

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5
#define STACK_SIZE 2048
#define MPU9250_ADDRESS 0x68
#define VL53L1X_ADDRESS 0x60
#define VL53L1X_ADDRESS_2 0x61
#define MOTOR_1_PIN 41
#define MOTOR_2_PIN 42
#define ON_OFF_BUTTON_PIN 19
#define LONG_PRESS_TIME 10000
#define SHORT_PRESS_TIME 10000

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
MPU9250 mpu;
std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2};
std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, VL53L1X_ADDRESS},  {&vl53_2, VL53L1X_ADDRESS_2}};
std::vector<std::pair<MPU9250*, int>> mpu_sensors = {{&mpu, MPU9250_ADDRESS}};
SensorData sensor_data;
vibrationMotor motor1(MOTOR_1_PIN); 
vibrationMotor motor2(MOTOR_2_PIN);  
ezButton onOffButton(ON_OFF_BUTTON_PIN);
vibrationPattern vib_pattern = vibrationPattern::shortBuzz;
TwoWire secondBus = TwoWire(1);

static int DistanceSensorDelay = 75;
bool calibration_needed = false;
bool system_calibrated = false;
bool mpu_updated = false;
bool is_system_on = false;
unsigned long pressed_time = 0;
unsigned long released_time = 0;
bool is_pressing = false;
bool is_long_press = false;


//Samples sensors data
void sampleSensorsData(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true) {
    if(is_system_on) {
      //samples distance sensors data
      for (int i = 0; i < distance_sensors.size(); i++) {
        if(!isVL53L1XSensorConnected(distance_sensors[i].second, &secondBus)) {
            Serial.print("Sensor: ");
            Serial.print(i+1);
            Serial.println(" not connected");
            continue;
        } else {
            
            if(distance_sensors[i].first->dataReady()) {
                int distance = distance_sensors[i].first->distance();
                if (distance == -1) {
                  //Serial.print(F("Couldn't get distance: "));
                  //Serial.println(distance_sensors[i].first->vl_status);
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
      //samples MPU data
      if (mpu_sensors[0].first->update()) {
        vTaskDelay(delay_in_ms);
        sensor_data.setPitch(mpu_sensors[0].first->getPitch());
        int yaw = mpu_sensors[0].first->getYaw();
        if(yaw < 0 ) {
          yaw = 360 + yaw;
        }
        sensor_data.setYaw(yaw);
        sensor_data.setRoll(mpu_sensors[0].first->getRoll());
        sensor_data.setAccelX(mpu_sensors[0].first->getLinearAccX());
        sensor_data.setAccelY(mpu_sensors[0].first->getLinearAccY());
        sensor_data.setAccelZ(mpu_sensors[0].first->getLinearAccZ());
        sensor_data.setlastUpdateTime(millis());
      }
    }
    vTaskDelay(delay_in_ms);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin(17,18);
  Wire.setClock(100000); // Set I2C clock speed to 100 kHz
  delay(100);
  secondBus.begin(15,16);
  secondBus.setClock(100000); // Set I2C clock speed to 100 kHz

  //onOffButton.setDebounceTime(50);

  while (!Serial) delay(10);

  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }
  // Initialize Distance measuring sensors
  initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second, &secondBus);
  initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &secondBus);  

  //Initializes MPU
  if (!mpu.setup(MPU9250_ADDRESS)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
          delay(5000);
      }
  }
  calibrateMPU(mpu, calibration_needed);
  if(calibration_needed) {
    delay(10000);
  }
  system_calibrated = true;
  //Creates threaded tasks
  xTaskCreate(sampleSensorsData, "sampleSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
}

void loop() {
  //button loop
  onOffButton.loop();
  //press detection
  if(onOffButton.isPressed()) {
    Serial.println("Press detected");
    pressed_time = millis();
    is_pressing = true;
    is_long_press = false;
  }
  //release detection: short press = on off toggle. long press = recalibrate system.
  if(onOffButton.isReleased()) {
    is_pressing = false;
    released_time = millis();

    long press_duration = released_time - pressed_time;

    if(press_duration < SHORT_PRESS_TIME) {
      Serial.println("Short press detected");
      if(is_system_on) {
        is_system_on = false;
        Serial.println("System shut down");
        motor1.vibrate(vibrationPattern::powerOFFBuzz);
      } else {
        is_system_on = true;
        Serial.println("Powering on system");
        motor1.vibrate(vibrationPattern::powerONBuzz);
      }
    }
  }
  if(is_pressing == true && is_long_press == false) {
    long press_duration = millis() - pressed_time;
    if(press_duration > LONG_PRESS_TIME) {
      Serial.println("Long press detected - Calibrating system");
      is_long_press = true;
      is_system_on = false;
      system_calibrated = false;
      calibration_needed = true;
      motor1.vibrate(vibrationPattern::recalibrationBuzz);
      calibrateMPU(mpu, calibration_needed);
      delay(10000);
      system_calibrated = true;
      is_system_on = true;
      calibration_needed = false;
    }
  }
  //sensor data update routine
  if (mpu.update() && system_calibrated && is_system_on) {
    sensor_data.printData();
    if(sensor_data.getDistanceSensor1() < 500 && sensor_data.getDistanceSensor2() < 500 && sensor_data.getDistanceSensor1() != -1 && sensor_data.getDistanceSensor2() != -1) {
      motor1.vibrate(vib_pattern);
      motor2.vibrate(vib_pattern);
    }
  }
  delay(50);
}
