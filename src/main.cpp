
#include <Arduino.h>
#include <vector>
#include <cstring>
#include <utility>
#include <ezButton.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Firebase_ESP_Client.h>
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
#include "camera.h"
#include "webmsg.h"

// system sensor objects
Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2);
Adafruit_VL53L1X vl53_3 = Adafruit_VL53L1X(XSHUT_PIN_3);
Adafruit_VL53L1X vl53_4 = Adafruit_VL53L1X(XSHUT_PIN_4);
MPU9250 mpu;
extern MP3 mp3;
static vibrationMotor motor1(MOTOR_1_PIN);
static vibrationMotor motor2(MOTOR_2_PIN);
static ezButton onOffButton(ON_OFF_BUTTON_PIN);

std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2, XSHUT_PIN_3, XSHUT_PIN_4};
std::vector<std::pair<Adafruit_VL53L1X *, int>> distance_sensors = {{&vl53_1, VL53L1X_ADDRESS}, {&vl53_2, VL53L1X_ADDRESS_2}, {&vl53_3, VL53L1X_ADDRESS_3}, {&vl53_4, VL53L1X_ADDRESS_4}};
static std::vector<std::pair<MPU9250 *, int>> mpu_sensors = {{&mpu, MPU9250_ADDRESS}};

static SensorData sensor_data;

TwoWire secondBus = TwoWire(1);

extern FirebaseData firebaseData; // Firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool wifi_flag = false;
bool sd_flag = false;
bool camera_flag = false;

extern systemSettings system_settings;
// bool save_flag = false;

WiFiClientSecure client;

static bool is_system_on = false;
static unsigned long pressed_time = 0;
static unsigned long released_time = 0;
static bool is_pressing = false;
static bool is_long_press = false;
static double velocity = 0.0;
static bool initial_powerup = true;
static int step_count = 0;
static unsigned long startTime, currTime;
static bool calibrate_flag = false;
static bool system_calibrated = false;
static bool calibration_needed = false;
unsigned long pressStartTime = 0;
unsigned long lastPressTime = 0;


// Samples sensors data
void sampleSensorsData(void *pvParameters)
{
  int delay_in_ms = *(int *)pvParameters;
  while (true)
  {
    if (is_system_on)
    {
      // samples distance sensors data
      for (int i = 0; i < distance_sensors.size(); i++)
      {
        if (!isVL53L1XSensorConnected(distance_sensors[i].second, &Wire))
        {
          Serial.print("Sensor: ");
          Serial.print(i + 1);
          Serial.println(" not connected");
          continue;
        }
        else
        {
          if (distance_sensors[i].first->dataReady())
          {
            int distance = distance_sensors[i].first->distance();
            if (distance == -1)
            {
              //Serial.print(F("Couldn't get distance: "));
              //Serial.println(distance_sensors[i].first->vl_status);
              if (i == 0)
              {
                sensor_data.setSensor1Distance(distance);
              }
              if (i == 1)
              {
                sensor_data.setSensor2Distance(distance);
              }
              if (i == 2)
              {
                sensor_data.setSensor3Distance(distance);
              }
              if (i == 3)
              {
                sensor_data.setSensor4Distance(distance);
              }
              continue;
            }
            else
            {
              if (i == 0)
              {
                sensor_data.setSensor1Distance(distance);
              }
              if (i == 1)
              {
                sensor_data.setSensor2Distance(distance);
              }
              if (i == 2)
              {
                sensor_data.setSensor3Distance(distance);
              }
              if (i == 3)
              {
                sensor_data.setSensor4Distance(distance);
              }
            }
            distance_sensors[i].first->clearInterrupt();
          }
          else
          {
            Serial.println(F("Data not ready"));
          }
        }
      }
      // samples MPU data
      if (mpu_sensors[0].first->update())
      {
        vTaskDelay(delay_in_ms);
        sensor_data.setPitch(mpu_sensors[0].first->getPitch());
        int yaw = mpu_sensors[0].first->getYaw();
        if (yaw < 0)
        {
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

void calculateVelocityAsTask(void *pvParameters)
{
  int delay_in_ms = *(int *)pvParameters;
  while (true)
  {
    if (is_system_on)
    {
      calculateStepCountAndSpeed(sensor_data, &step_count, &velocity, 1.75);
    }
    vTaskDelay(delay_in_ms);
  }
}

void systemInit()
{
  Serial.println("--------- System Init ---------");
  if (!wifi_flag)
    wifi_flag = WifiSetup();
  if (wifi_flag){
    mp3.playWithFileName(VOICE_ALERTS_DIR, WIFI_CONNECTED);
    delay(1000);
  }
    else{
    mp3.playWithFileName(VOICE_ALERTS_DIR, WIFI_NOT_CONNECTED);
    delay(2000);
  }
  if (!sd_flag)
  {
    if (setupSDCard())
    {
      init_sd_card();
      
      system_settings = readSettings(SD_MMC, "/Settings/setting.txt");
      sd_flag = true;
      // Serial.println("----------------");
      // system_settings.print();
    }

    else
    {
      Serial.println("Failed to initialize SD card");
      mp3.playWithFileName(VOICE_ALERTS_DIR, NO_SD_DETECTED);
      delay(4000);
      sd_flag = false;
    }
  }
  // if we managed to connect to WIFI - use firebase settings, as they are the most updated.
  if (wifi_flag)
  {
    if (initial_powerup)
    {
      setupFirebase(config, auth);
      initial_powerup = false;
    }
    // storeFirebaseSetting(&firebaseData,system_settings);
    systemSettings system_settings_from_fb;
    if (getFirebaseSettings(&firebaseData, system_settings_from_fb))
    {
      if (system_settings.updateSettings(system_settings_from_fb))
      {
        system_settings.print();
        if (sd_flag)
          updateSDSettings(system_settings);
      }
    }
    updateFirebaseLocalIP(&firebaseData, WiFi.localIP().toString());
    setupWifiServer();
    setupMsgServer();
    setupTime();
    // createDir(SD_MMC,"/images");
  }
  else
  {
    initial_powerup = true;
  }
  is_system_on = true;

  if(!camera_flag) {
    camera_flag = setupCamera();
    if (!camera_flag) {
      mp3.playWithFileName(VOICE_ALERTS_DIR, NO_CAMERA_DETECTED);
      delay(3000);
    }
  }

  if(!wifi_flag && !sd_flag){
    mp3.playWithFileName(VOICE_ALERTS_DIR, NO_SD_AND_WIFI);
    delay(7000);
  }

  Serial.printf("--------- System Init Done ---------\n");
}


void setup()
{

  static int DistanceSensorDelay = 50;
  static int SpeedCalcDelay = 100;
  delay(500);
  mp3.playWithFileName(VOICE_ALERTS_DIR, POWERING_ON_SYSTEM);
  delay(200);
  Serial.begin(115200);
  delay(100);
  Wire.begin(3, 14);
  Wire.setClock(100000); // Set I2C clock speed to 100 kHz
  delay(100);
  // secondBus.begin(35, 36);
  // secondBus.setClock(400000); // Set I2C clock speed to 100 kHz
  Serial.println("Starting setup");
  // onOffButton.setDebounceTime(50);

  while (!Serial)
    delay(10);
  // Initialize XSHUT pins
  for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++)
  {
    pinMode(distance_sensors_xshut_pins[i], OUTPUT);
  }
  // sets mp3 initial volume
  //mp3.setVolume(0x15);
  // Initialize Distance measuring sensors
  initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second, &Wire);
  initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &Wire);
  initializeVL53L1XSensor(distance_sensors[2].first, XSHUT_PIN_3, distance_sensors[2].second, &Wire);
  initializeVL53L1XSensor(distance_sensors[3].first, XSHUT_PIN_4, distance_sensors[3].second, &Wire);

  MPU9250Setting mpu_setting;
  mpu_setting.accel_fs_sel = ACCEL_FS_SEL::A2G;
  mpu_setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
  mpu_setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  mpu_setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
  mpu_setting.gyro_fchoice = 0x03;
  mpu_setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_5HZ;
  mpu_setting.accel_fchoice = 0x01;
  mpu_setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_5HZ;

  // Initializes MPU
  if (!mpu.setup(MPU9250_ADDRESS, mpu_setting))
  { // change to your own address
    while (1)
    {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }
  calibrateMPU(&mpu, calibration_needed, &mp3);

  if (calibration_needed)
  {
    delay(10000);
  }
  system_calibrated = true;
  systemInit();
  Serial.println("SAFE STEP IS READY TO USE: STARTING OPERATIONS");
  mp3.playWithFileName(VOICE_ALERTS_DIR, SYSTEM_READY_TO_USE);
  delay(200);
  // Creates threaded tasks
  xTaskCreate(sampleSensorsData, "sampleSensorsData", STACK_SIZE, &DistanceSensorDelay, 2, nullptr);
  xTaskCreate(calculateVelocityAsTask, "calculateVelocity", STACK_SIZE, &SpeedCalcDelay, 2, nullptr);
}

void loop()
{

onOffButton.loop(); // Update button state

static bool is_double_press_pending = false; // Flag to track potential double press
static unsigned long double_press_start_time = 0; // Tracks time of the first press in a double press

// Press detection
if (onOffButton.isPressed())
{
    Serial.println("Press detected");
    pressed_time = millis();
    is_pressing = true;
    is_long_press = false;
}

// Release detection
if (onOffButton.isReleased())
{
    is_pressing = false;
    unsigned long pressDuration = millis() - pressed_time; // Calculate press duration
    unsigned long currentTime = millis();

    if (pressDuration >= LONG_PRESS_THRESHOLD)
    {
        // Long press detected
        Serial.println("Long press detected");
        Serial.println("SAFESTEP MPU RECALIBRATION ROUTINE STARTING...");
        is_long_press = true; // Set long press flag
        system_calibrated = false;
        calibration_needed = true;
        motor1.vibrate(vibrationPattern::recalibrationBuzz);
        calibrateMPU(&mpu, calibration_needed, &mp3);
        delay(10000);
        system_calibrated = true;
        calibration_needed = false;
        is_long_press = false;

        // Reset double press tracking
        is_double_press_pending = false;
    }
    else
    {
        if (is_double_press_pending)
        {
            // Confirmed double press
            Serial.println("Double press detected");
            mp3.playWithFileName(VOICE_ALERTS_DIR, WIFI_PAIRING_INITIATED);
            delay(100);
            Serial.println("SAFESTEP PAIRING PROCEDURE STARTED - PAIRING TO A NEW WIFI NETWORK...");
            motor1.vibrate(vibrationPattern::pulseBuzz);
            if(!WifiManagerSetup()) {
              Serial.println("Failed to connect to a new network, using SD card settings instead...");
            } else {
              wifi_flag = true;
              systemInit();
            }; // Perform double press action
            // Reset double press tracking
            is_double_press_pending = false;
        }
        else
        {
            // First press of a potential double press
            is_double_press_pending = true;
            double_press_start_time = currentTime;
        }
    }
}

// Check for short press after double press timeout
if (is_double_press_pending && (millis() - double_press_start_time > DOUBLE_PRESS_THRESHOLD))
{
    // No second press detected, treat as a short press
    Serial.println("Short press detected");
    Serial.println("SAFESTEP SHORT PRESS ROUTINE STARTING...");
    motor1.vibrate(vibrationPattern::shortBuzz);

    // Toggle system mode
    String curr_mode = system_settings.getMode();
    if (curr_mode == "Both" || curr_mode == "Sound")
    {
        system_settings.setMode("Vibration");
        mp3.playWithFileName(VOICE_ALERTS_DIR, SILENT_MODE_ACTIVATED);
        delay(100);
    }
    else
    {
        system_settings.setMode("Both");
        mp3.playWithFileName(VOICE_ALERTS_DIR, SILENT_MODE_DEACTIVATED);
        delay(100);

    }

    // Reset double press tracking
    is_double_press_pending = false;
}
  if(is_system_on && !is_pressing && system_calibrated) {
    // sensor data update routine
    if(system_settings.getAlertMethod() == "TimeToImpact") {
        if (mpu.update() && system_calibrated && is_system_on && !is_pressing) {  
            sensor_data.printData();
            double nearest_obstacle_collision_time = nearestObstacleCollisionTime(sensor_data, system_settings, &velocity);
            if(collisionTimeAlertHandler(nearest_obstacle_collision_time, system_settings, mp3, motor1)) {
              if(system_settings.getEnableCamera()){
                CaptureObstacle(fbdo, auth, config, wifi_flag);
              }
            }
        }
    } else {
          if (is_system_on && !is_pressing) {
            double nearest_obstacle_distance = distanceToNearestObstacle(sensor_data, system_settings, &velocity);
            if(obstacleDistanceAlertHandler(nearest_obstacle_distance, system_settings, mp3, motor1)) {
              if(system_settings.getEnableCamera()){
                CaptureObstacle(fbdo, auth, config, wifi_flag);
              }
            }  
          }              
    }
  }
  system_settings.changeVolume(system_settings.getVolume(), &mp3);
  vTaskDelay(50);
}
