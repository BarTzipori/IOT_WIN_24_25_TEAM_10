
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

struct VelocityTaskParams
{
  int delay_in_ms;          // Delay in milliseconds
  systemSettings *settings; // Pointer to systemSettings object
  double *velocity;         // Pointer to the velocity variable
  int *step_count;
};

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
Flags flags;

systemSettings system_settings;
// bool save_flag = false;

WiFiClientSecure client;

static bool is_system_on = false;
static unsigned long pressed_time = 0;
static unsigned long released_time = 0;
static bool is_pressing = false;
static bool is_long_press = false;
static double velocity = 0.0;
static int step_count = 0;
static unsigned long startTime, currTime;
static bool calibrate_flag = false;
static bool system_calibrated = false;
static bool calibration_needed = false;
static bool mpu_degraded_flag = false;
unsigned long pressStartTime = 0;
unsigned long lastPressTime = 0;

// Samples sensors data
void sampleSensorsData(void *pvParameters)
{
  int delay_in_ms = *(int *)pvParameters;
  int distance = 0;
  bool distance_sensor_degraded_notification_flag = true;
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
          distance = -1;
          if (distance_sensor_degraded_notification_flag)
          {
            mp3.playWithFileName(VOICE_ALERTS_DIR, DISTANCE_SENSOR_DEGRADED);
            String log_data = "ERROR: ONE OR MORE DISTANCE MEASURING SENSORS NOT CONNECTED - OPERATING IN DEGRADED MODE";
            vTaskDelay(3000);
            distance_sensor_degraded_notification_flag = false;
          }
        }
        else
        {
          vTaskDelay(50);
          if (distance_sensors[i].first->dataReady())
          {
            distance = distance_sensors[i].first->distance();
          }
          else
          {
            Serial.println(F("Data not ready"));
          }
        }
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
        distance_sensors[i].first->clearInterrupt();
      }
    }
    // samples MPU data
    if (!mpu_degraded_flag && mpu_sensors[0].first->update())
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
    }
    sensor_data.setlastUpdateTime(millis());
  }
  vTaskDelay(delay_in_ms);
}

void calculateVelocityAsTask(void *pvParameters)
{
  // Cast the void* parameter to VelocityTaskParams*
  VelocityTaskParams *params = (VelocityTaskParams *)pvParameters;
  int delay_in_ms = params->delay_in_ms;
  systemSettings *settings = params->settings;
  double *velocity = params->velocity;
  int *step_count = params->step_count;
  float user_height_in_meters = settings->getUserHeight() / 100;

  while (true)
  {
    if (is_system_on)
    {
      calculateStepCountAndSpeed(sensor_data, step_count, velocity, user_height_in_meters);
    }
    vTaskDelay(delay_in_ms);
  }
}

void systemInit()
{
  Serial.println("--------- System Init ---------");
  if (!flags.wifi_flag)
  {
    flags.wifi_flag = WifiSetup();
  }
  if (flags.wifi_flag)
  {
    setupTime();
    mp3.playWithFileName(VOICE_ALERTS_DIR, WIFI_CONNECTED);
    delay(1000);
  }
  else
  {
    mp3.playWithFileName(VOICE_ALERTS_DIR, WIFI_NOT_CONNECTED);
    delay(2000);
  }
  if (!flags.sd_flag)
  {
    if (setupSDCard())
    {
      init_sd_card();
      init_logs(flags.wifi_flag);
      system_settings = readSettings(SD_MMC, "/Settings/setting.txt");
      flags.sd_flag = true;
      // Serial.println("----------------");
      // system_settings.print();
    }
    else
    {
      Serial.println("Failed to initialize SD card");
      mp3.playWithFileName(VOICE_ALERTS_DIR, NO_SD_DETECTED);
      delay(4000);
      flags.sd_flag = false;
    }
  }
  // if we managed to connect to WIFI - use firebase settings, as they are the most updated.
  if (flags.wifi_flag)
  {
    setupFirebase(config, auth);
    // storeFirebaseSetting(&firebaseData,system_settings);
    systemSettings system_settings_from_fb;
    if (getFirebaseSettings(&firebaseData, system_settings_from_fb))
    {
      if (system_settings.updateSettings(system_settings_from_fb))
      {
        system_settings.print();
        if (flags.sd_flag)
          updateSDSettings(system_settings);
      }
    }
    updateFirebaseLocalIP(&firebaseData, WiFi.localIP().toString());
    setupWifiServer();
    setupMsgServer();
    setupTime();
    // createDir(SD_MMC,"/images");
  }
  if (!flags.camera_flag)
  {
    flags.camera_flag = setupCamera();
    if (!flags.camera_flag)
    {
      mp3.playWithFileName(VOICE_ALERTS_DIR, NO_CAMERA_DETECTED);
      delay(3000);
    }
  }

  if (!flags.wifi_flag && !flags.sd_flag)
  {
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

  systemInit();

  // Initializes MPU

  Serial.println("SAFE STEP IS READY TO USE: STARTING OPERATIONS");
  String log_data = "SAFE STEP IS READY TO USE: STARTING OPERATIONS";
  logData(log_data);
  mp3.playWithFileName(VOICE_ALERTS_DIR, SYSTEM_READY_TO_USE);
  delay(2000);
  is_system_on = true;
  // Creates threaded tasks
  uploadLogs(SD_MMC, fbdo, auth, config);
  uploadImages(SD_MMC, fbdo, auth, config);
}

void loop()
{
  if (flags.wifi_flag)
  {
    wifiServerLoop();
    msgServerLoop();
  }
}
