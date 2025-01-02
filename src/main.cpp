
#include <Arduino.h>
#include <vector>
#include <cstring>
#include <utility>
#include <ezButton.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
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
#include "sd_read_write.h"
#include "SD_MMC.h"
#include "systemSettings.h"

#define IRQ_PIN 2
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 5
#define XSHUT_PIN_3 6
#define XSHUT_PIN_4 7
#define STACK_SIZE 2048
#define MPU9250_ADDRESS 0x68
#define VL53L1X_ADDRESS 0x60
#define VL53L1X_ADDRESS_2 0x61
#define VL53L1X_ADDRESS_3 0x52
#define VL53L1X_ADDRESS_4 0x53
#define MOTOR_1_PIN 46
#define MOTOR_2_PIN 46
#define MP3_RX 12
#define MP3_TX 11
#define ON_OFF_BUTTON_PIN 20
#define LONG_PRESS_TIME 10000
#define SHORT_PRESS_TIME 10000
#define OBSTACLE_DISTANCE 300

#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

#define WIFI_TIMEOUT 1500
// Replace with your network credentials
#define WIFI_SSID "Matans iPhone"
#define WIFI_PASSWORD "12345678B"
// Firebase credentials
#define FIREBASE_HOST "https://safestep-2bc31-default-rtdb.europe-west1.firebasedatabase.app" // Replace with your database URL
#define API_KEY "AIzaSyBplNk42Hp3O6M17iJObv_FM2MuanadKjM"
#define USER_EMAIL "neta.matano@gmail.com"
#define USER_PASSWORD "123456"

Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1, IRQ_PIN);
Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2, IRQ_PIN);
Adafruit_VL53L1X vl53_3 = Adafruit_VL53L1X(XSHUT_PIN_3, IRQ_PIN);
Adafruit_VL53L1X vl53_4 = Adafruit_VL53L1X(XSHUT_PIN_4, IRQ_PIN);
MPU9250 mpu;
MP3 mp3(MP3_RX, MP3_TX);
vibrationMotor motor1(MOTOR_1_PIN); 
vibrationMotor motor2(MOTOR_2_PIN);  
ezButton onOffButton(ON_OFF_BUTTON_PIN);

std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2, XSHUT_PIN_3, XSHUT_PIN_4};
std::vector<std::pair<Adafruit_VL53L1X*, int>> distance_sensors = {{&vl53_1, VL53L1X_ADDRESS},  {&vl53_2, VL53L1X_ADDRESS_2}};
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

int8_t volume = 0x1a;//0~0x1e (30 adjustable level)
int8_t folderName = 0x01;//folder name must be 01 02 03 04 ...
int8_t fileName = 0x01; // prefix of file name must be 001xxx 002xxx 003xxx 004xxx ...

static int DistanceSensorDelay = 50;
bool calibration_needed = false;
bool system_calibrated = false;
bool mpu_updated = false;
bool is_system_on = false;
unsigned long pressed_time = 0;
unsigned long released_time = 0;
bool is_pressing = false;
bool is_long_press = false;
float velocity = 0;


//firebase functions
bool setupSDCard()
{
        SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
      Serial.println("Card Mount Failed");
      return false;
    }
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return false;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
    return true;
}

bool WifiSetup()
{
    startTime = millis();
    currTime = millis();
    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED && currTime - startTime < WIFI_TIMEOUT)
    {
        delay(1000);
        Serial.print(".");
        currTime = millis();
        // Serial.println(currTime - startTime);
  }
  if(WiFi.status()==WL_CONNECTED){
      Serial.println("\nConnected to Wi-Fi");
      return true;
  }
      else {
        Serial.println("\nNot Connected!");
        return false;
      }
}

void init_sd_card()
{
  if (isExist(SD_MMC, "/Settings", "setting.txt"))
    Serial.println("setting file exist!");
    else  {
        createDir(SD_MMC, "/Settings");
        writeFile(SD_MMC, "/Settings/setting.txt", "Mode: Both\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Sound: Sound_1\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Viberation: Viberation_1\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Timing: 0.5s\n");
        appendFile(SD_MMC, "/Settings/setting.txt", "Height: 1.65\n");
        endFile(SD_MMC, "/Settings/setting.txt");
        Serial.println("created setting file");
    }
}

void setupFirebase(){
  config.api_key = API_KEY;
  config.database_url = FIREBASE_HOST;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  config.token_status_callback = tokenStatusCallback;
}

systemSettings getFirebaseSettings() {
  
  String mode, viberation, timming, sound;
  double height;
  if (Firebase.getString(firebaseData, "/System_Settings/Vibration"))
  {
    viberation = firebaseData.stringData();
  } else {
    Serial.print("Failed to get viberation: ");
    Serial.println(firebaseData.errorReason());
}

if (Firebase.getString(firebaseData, "/System_Settings/Sound")) {
    sound = firebaseData.stringData();
    //Serial.println("Sound: " + sound);
  } else {
      Serial.print("Failed to get sound: ");
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Mode")) {
    mode = firebaseData.stringData();
    //Serial.println("Mode: " + mode);
  } else {
      Serial.print("Failed to get mode: "); 
      Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/System_Settings/Timing")) {
    timming = firebaseData.stringData();
    //Serial.println("Timing: " + timming);
  } else {
      Serial.print("Failed to get timing: ");
      Serial.println(firebaseData.errorReason());
  }
  
    if (Firebase.getDouble(firebaseData, "/System_Settings/Height")) {
    height = firebaseData.doubleData();
    //Serial.print("Height: ");
    // Serial.println(height);
    } else {
      Serial.print("Failed to get height: ");
      Serial.println(firebaseData.errorReason());
  }
  return systemSettings(mode, sound, viberation, timming, height);
}

void storeFirebaseSetting(systemSettings& s)
{
   if (Firebase.setString(firebaseData, "/System_Settings/Sound", s.getSound())) {
      Serial.println("sound stored successfully");
    } else {
      Serial.print("Error storing sound: ");
      Serial.println(firebaseData.errorReason());
    }

  if (Firebase.setString(firebaseData, "/System_Settings/Vibration", s.getViberation())) {
    Serial.println("vibration stored successfully");
  } else {
    Serial.print("Error storing vibration: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setString(firebaseData, "/System_Settings/Mode", s.getMode())) {
    Serial.println("mode stored successfully");
  } else {
    Serial.print("Error storing mode: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setString(firebaseData, "/System_Settings/Timing",s.getTiming())) {
    Serial.println("timing stored successfully");
  } else {
    Serial.print("Error storing timing: ");
    Serial.println(firebaseData.errorReason());
  }

   if (Firebase.setInt(firebaseData, "/System_Settings/Height", s.getHeight())) {
    Serial.println("height stored successfully");
  } else {
    Serial.print("Error storing height: ");
    Serial.println(firebaseData.errorReason());
  }
}

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
    calculateVelocity(sensor_data, velocity, 5);
    Serial.print("Velocity: ");
    Serial.println(velocity);
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
  initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &secondBus);  
  //initializeVL53L1XSensor(distance_sensors[2].first, XSHUT_PIN_3, distance_sensors[2].second, &secondBus);
  //initializeVL53L1XSensor(distance_sensors[3].first, XSHUT_PIN_4, distance_sensors[3].second, &secondBus);

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
  Serial.println("Waiting for system to be powered on");
  //Creates threaded tasks
  xTaskCreate(sampleSensorsData, "sampleSensorsData", STACK_SIZE, &DistanceSensorDelay, 3, nullptr);
  //xTaskCreate(calculateVelocityAsTask, "calculateVelocity", STACK_SIZE, &DistanceSensorDelay, 1, nullptr);
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
        Serial.println("Powering on system");
        motor1.vibrate(vibrationPattern::powerONBuzz);
        velocity = 0;
        // attempt to connect to wifi
        wifi_flag = WifiSetup();
        // Initialize Firebase
        setupSDCard();
        init_sd_card();
        system_settings = readSettings(SD_MMC, "/Settings/setting.txt");
        //if we managed to connect to WIFI - use firebase settings, as they are the most updated.
        if (wifi_flag){
          setupFirebase();
          systemSettings system_settongs_from_fb = getFirebaseSettings();
          system_settings.updateSettings(system_settongs_from_fb);
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
  //sensor data update routine
  if (mpu.update() && system_calibrated && is_system_on) {
    sensor_data.printData();
    if((sensor_data.getDistanceSensor1() < OBSTACLE_DISTANCE && sensor_data.getDistanceSensor1() != -1) || (sensor_data.getDistanceSensor2() < OBSTACLE_DISTANCE && sensor_data.getDistanceSensor2() != -1)) {
      Serial.println(system_settings.getMode());
      if(system_settings.getMode() == "Viberation") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &motor2, 1, nullptr);
      }
      if(system_settings.getMode() == "Sound") {
        static void* audio_params[3];
        audio_params[0] = (void*)&mp3;                  // pointer to MP3
        audio_params[1] = (void*)(uintptr_t)0x01;       //dir name
        audio_params[2] = (void*)(uintptr_t)0x01;       //file name
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1000);
      }
      if(system_settings.getMode() == "Both") {
        xTaskCreate(vibrateMotorsAsTask, "vibrateMotor1", STACK_SIZE, &motor2, 1, nullptr);
        static void* audio_params[3];
        audio_params[0] = (void*)&mp3;                  // pointer to MP3
        audio_params[1] = (void*)(uintptr_t)0x01;       //dir name
        audio_params[2] = (void*)(uintptr_t)0x01;       //file name
        xTaskCreate(playMP3AsTask, "playmp3", STACK_SIZE, audio_params, 4, nullptr);
        vTaskDelay(1000);
      }
    }
  }
  vTaskDelay(50);
}
