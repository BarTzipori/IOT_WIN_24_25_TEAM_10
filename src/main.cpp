
#include <Arduino.h>
#include <vector>
#include <cstring>
#include <utility>
#include <ezButton.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseESP32.h>
#include <Arduino.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "Adafruit_VL53L1X.h"
#include "MPU9250.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sensorData.h"
#include "sensorHelperFunctions.h"
#include "vibrationMotor.h"
#include "RedMP3.h"
#include "collisionDetectionAlgorithm.h"
#include "sdCardHelperFunctions.h"
#include "systemSettings.h"
#include "parameters.h"
#include "commHelperFunctions.h"
#include "wifiServer.h"

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
Adafruit_VL53L1X vl53_3 = Adafruit_VL53L1X(XSHUT_PIN_3, IRQ_PIN);
Adafruit_VL53L1X vl53_4 = Adafruit_VL53L1X(XSHUT_PIN_4, IRQ_PIN);
MPU9250 mpu;
extern MP3 mp3;
vibrationMotor motor1(MOTOR_1_PIN);
vibrationMotor motor2(MOTOR_2_PIN);  
ezButton onOffButton(ON_OFF_BUTTON_PIN);

std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2, XSHUT_PIN_3, XSHUT_PIN_4};
std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, VL53L1X_ADDRESS},  {&vl53_2, VL53L1X_ADDRESS_2}, {&vl53_3, VL53L1X_ADDRESS_3}, {&vl53_4, VL53L1X_ADDRESS_4}};
std::vector<std::pair<MPU9250*, int>> mpu_sensors = {{&mpu, MPU9250_ADDRESS}};

SensorData sensor_data;
//default vibration pattern
vibrationPattern vib_pattern = vibrationPattern::shortBuzz;
TwoWire secondBus = TwoWire(1);

FirebaseData firebaseData; // Firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long startTime,currTime;
bool wifi_flag;
systemSettings system_settings;
//bool save_flag = false;

int8_t volume = 0x1a;//0~0x1e (30 adjustable level)
int8_t folderName = 0x01;//folder name must be 01 02 03 04 ...
int8_t fileName = 0x01; // prefix of file name must be 001xxx 002xxx 003xxx 004xxx ...

WiFiClientSecure client;

static int DistanceSensorDelay = 50;
static int SpeedCalcDelay = 250;
bool calibration_needed = false;
bool system_calibrated = false;
bool mpu_updated = false;
bool is_system_on = false;
unsigned long pressed_time = 0;
unsigned long released_time = 0;
bool is_pressing = false;
bool is_long_press = false;
double velocity = 0.0;
bool initial_powerup = true;
int step_count = 0;

//Samples sensors data
void sampleSensorsData(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true) {
    if(is_system_on) {
      //samples distance sensors data
      for (int i = 0; i < distance_sensors.size(); i++) {
        if(!isVL53L1XSensorConnected(distance_sensors[i].second, &secondBus)) {
            //Serial.print("Sensor: ");
            //Serial.print(i+1);
            //Serial.println(" not connected");
            continue;
        } else {     
            if(distance_sensors[i].first->dataReady()) {
                int distance = distance_sensors[i].first->distance();
                if (distance == -1) {
                  //Serial.print(F("Couldn't get distance: "));
                  //Serial.println(distance_sensors[i].first->vl_status);
                  if(i == 0) {
                    sensor_data.setSensor1Distance(distance);
                  }
                  if(i == 1) {
                    sensor_data.setSensor2Distance(distance);
                  }
                  if(i == 2) {
                    sensor_data.setSensor3Distance(distance);
                  }
                  if(i == 3) {
                    sensor_data.setSensor4Distance(distance);
                  }
                  continue;
                } else {
                  if(i == 0) {
                    sensor_data.setSensor1Distance(distance);
                  }
                  if(i == 1) {
                    sensor_data.setSensor2Distance(distance);
                  }
                  if(i == 2) {
                    sensor_data.setSensor3Distance(distance);
                  }
                  if(i == 3) {
                    sensor_data.setSensor4Distance(distance);
                  }
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
        sensor_data.setAccelX(mpu_sensors[0].first->getAccX());
        sensor_data.setAccelY(mpu_sensors[0].first->getAccY());
        sensor_data.setAccelZ(mpu_sensors[0].first->getAccZ());
        sensor_data.setLinearAccelX(mpu_sensors[0].first->getLinearAccX());
        sensor_data.setLinearAccelY(mpu_sensors[0].first->getLinearAccY());
        sensor_data.setLinearAccelZ(mpu_sensors[0].first->getLinearAccZ());
        sensor_data.setGyroX(mpu_sensors[0].first->getGyroX());
        sensor_data.setGyroY(mpu_sensors[0].first->getGyroY());
        sensor_data.setGyroZ(mpu_sensors[0].first->getGyroZ());
        sensor_data.updateLinearAccelX();
        sensor_data.setlastUpdateTime(millis());
      }
    }
    vTaskDelay(delay_in_ms);
  }
}

void vibrateMotorsAsTask(void *pvParameters) {
  vibrationMotor* motor = (vibrationMotor*)pvParameters;
  motor->vibrate(vib_pattern);
  vTaskDelete(NULL);
}

void playMP3AsTask(void *pvParameters) {
  // Cast the incoming parameter to an array of void pointers
  void** params = (void**) pvParameters;

  MP3* mp3 = (MP3*) params[0];
  uint8_t directory_name = (uint8_t)(uintptr_t)params[1];
  uint8_t file_name  = (uint8_t)(uintptr_t)params[2];

  mp3->playWithFileName(directory_name, file_name);
  vTaskDelay(1000);
  // Task is done, so delete itself
  vTaskDelete(NULL);
}
void calculateVelocityAsTask(void *pvParameters) {
  int delay_in_ms = *(int *)pvParameters;
  while(true) {
    if(is_system_on) {
      //calculateVelocity(sensor_data, &velocity, delay_in_ms);
      //calculateVelocityWithZUPT(sensor_data, &velocity, delay_in_ms);
      //calculateHorizonVelocityWithZUPT(sensor_data, &velocity, delay_in_ms);
      //calculateHorizonVelocityWithZUPT2(sensor_data, &velocity, delay_in_ms);
      calculateStepCount(sensor_data, &step_count);

    }
    vTaskDelay(delay_in_ms);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin(41,42);
  Wire.setClock(100000); // Set I2C clock speed to 100 kHz
  delay(100);
  secondBus.begin(15,16);
  secondBus.setClock(400000); // Set I2C clock speed to 100 kHz
  Serial.println("Starting setup");
  //onOffButton.setDebounceTime(50);

  while (!Serial) delay(10);

  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++) {
      pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }
  //sets mp3 initial volume
  mp3.setVolume(0x15);
  // Initialize Distance measuring sensors
  initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second, &secondBus);
  //initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &secondBus);  
  initializeVL53L1XSensor(distance_sensors[2].first, XSHUT_PIN_3, distance_sensors[2].second, &secondBus);
  initializeVL53L1XSensor(distance_sensors[3].first, XSHUT_PIN_4, distance_sensors[3].second, &secondBus);

  MPU9250Setting mpu_setting;
  mpu_setting.accel_fs_sel = ACCEL_FS_SEL::A2G;
  mpu_setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
  mpu_setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  mpu_setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
  mpu_setting.gyro_fchoice = 0x03;
  mpu_setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_5HZ;
  mpu_setting.accel_fchoice = 0x01;
  mpu_setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_5HZ;

  //Initializes MPU
  if (!mpu.setup(MPU9250_ADDRESS, mpu_setting)) {  // change to your own address
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
  Serial.println("Waiting for system to be powered on");
  //Creates threaded tasks
  xTaskCreate(sampleSensorsData, "sampleSensorsData", STACK_SIZE, &DistanceSensorDelay, 2, nullptr);
  xTaskCreate(calculateVelocityAsTask, "calculateVelocity", STACK_SIZE, &SpeedCalcDelay, 2, nullptr);
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
    //short press - on off toggle
    if(press_duration < SHORT_PRESS_TIME) {
      // on/off routine
      Serial.println("Short press detected");
      if(is_system_on) {
        is_system_on = false;
        client.stop();
        WiFi.disconnect();
        Firebase.reset(&config);
        Serial.println("System shut down");
        motor1.vibrate(vibrationPattern::powerOFFBuzz);
      } else {
        Serial.println("Powering on system");
        motor1.vibrate(vibrationPattern::powerONBuzz);
        velocity = 0;
        // attempt to connect to wifi
        wifi_flag = WifiSetup(startTime, currTime);
        // Initialize Firebase
        bool flag1 = setupSDCard();
        bool flag2 = init_sd_card();
        system_settings = readSettings(SD_MMC, "/Settings/setting.txt");
        //if we managed to connect to WIFI - use firebase settings, as they are the most updated.
        if (wifi_flag){
          if(initial_powerup) {
            setupFirebase(config, auth);
            initial_powerup = false;
          }
          systemSettings system_settings_from_fb = getFirebaseSettings(firebaseData);
          system_settings.updateSettings(system_settings_from_fb);
          system_settings.print();
          updateSDSettings(system_settings);
          setupWifiServer();
        }
        else
        {
          initial_powerup = true;
        }
        is_system_on = true;
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
      calibration_needed = false;
      is_system_on = true;
    }
  }
  wifiServerLoop();
  // sensor data update routine
  if (mpu.update() && system_calibrated && is_system_on && !is_pressing) {
    //sensor_data.printData();
    if((sensor_data.getDistanceSensor1() < OBSTACLE_DISTANCE && sensor_data.getDistanceSensor1() > 0 )|| (sensor_data.getDistanceSensor2() < OBSTACLE_DISTANCE && sensor_data.getDistanceSensor2() > 0)) {
      Serial.println("Obstacle detected");
      if(system_settings.getMode() == "Vibration") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &motor2, 1, nullptr);
        vTaskDelay(1500);
      }
      if(system_settings.getMode() == "Sound") {
        static void* audio_params[3];
        audio_params[0] = (void*)&mp3;                  // pointer to MP3
        audio_params[1] = (void*)(uintptr_t)0x06;       //dir name
        audio_params[2] = (void*)(uintptr_t)0x03;       //file name
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
      }
      if(system_settings.getMode() == "Both") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &motor2, 1, nullptr);
        static void* audio_params[3];
        audio_params[0] = (void*)&mp3;                  // pointer to MP3
        audio_params[1] = (void*)(uintptr_t)0x06;       //dir name
        audio_params[2] = (void*)(uintptr_t)0x03;       //file name
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1500);
      }
    }
  }
  vTaskDelay(50);
}
