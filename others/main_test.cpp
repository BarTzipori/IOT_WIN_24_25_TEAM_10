
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
#include "voiceAlertsAsTasks.h"


// system sensor objects
static Adafruit_VL53L1X vl53_1 = Adafruit_VL53L1X(XSHUT_PIN_1);
static Adafruit_VL53L1X vl53_2 = Adafruit_VL53L1X(XSHUT_PIN_2);
static Adafruit_VL53L1X vl53_3 = Adafruit_VL53L1X(XSHUT_PIN_3);
static Adafruit_VL53L1X vl53_4 = Adafruit_VL53L1X(XSHUT_PIN_4);
MPU9250 mpu;
extern MP3 mp3;
static vibrationMotor motor1(MOTOR_1_PIN);
static ezButton onOffButton(ON_OFF_BUTTON_PIN);

static std::vector<int> distance_sensors_xshut_pins = {XSHUT_PIN_1, XSHUT_PIN_2, XSHUT_PIN_3, XSHUT_PIN_4};
static std::vector<std::pair<Adafruit_VL53L1X *, int>> distance_sensors = {{&vl53_1, VL53L1X_ADDRESS}, {&vl53_2, VL53L1X_ADDRESS_2}, {&vl53_3, VL53L1X_ADDRESS_3}, {&vl53_4, VL53L1X_ADDRESS_4}};
static std::vector<std::pair<MPU9250 *, int>> mpu_sensors = {{&mpu, MPU9250_ADDRESS}};

static SensorData sensor_data;
systemSettings system_settings;

extern FirebaseData firebaseData; // Firebase object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
Flags flags;


static bool is_system_on = false;
static unsigned long pressed_time = 0;
static bool is_pressing = false;
static bool is_long_press = false;
static double velocity = 0.0;
static int step_count = 0;
static bool system_calibrated = false;
static bool calibration_needed = false;
static bool mpu_degraded_flag = false;

void systemInit()
{
    Serial.println("--------- System Init ---------");
    if (!flags.wifi_flag) {
        flags.wifi_flag = WifiSetup();
    }
    if (flags.wifi_flag) {
        setupTime();
        xTaskCreate(playWifiConnectedAsTask, "playWifiConnectedAsTask", STACK_SIZE, &mp3, 2, nullptr);
    } else {
        xTaskCreate(playWifiNotConnectedAsTask, "playWifiNotConnectedAsTask", STACK_SIZE, &mp3, 2, nullptr);
    }
    if (!flags.sd_flag)
    {
        if (setupSDCard())
        {
            init_sd_card();
            init_logs(flags.wifi_flag);
            system_settings = readSettings(SD_MMC, "/Settings/setting.txt");
            flags.sd_flag = true;
        } else {
            String log_data = "ERROR: FAILED TO INITIALIZE SD CARD - SYSTEM WILL OPERATE IN DEGRADED MODE";
            logData(log_data);
            xTaskCreate(playNoSDDetectedAsTask, "playNoSDDetectedAsTask", STACK_SIZE, &mp3, 2, nullptr);
            flags.sd_flag = false;
        }
    }
    // if we managed to connect to WIFI - use firebase settings, as they are the most updated.
    if (flags.wifi_flag) {
        setupFirebase(config, auth);
        // storeFirebaseSetting(&firebaseData,system_settings);
        systemSettings system_settings_from_fb;
        if (getFirebaseSettings(&firebaseData, system_settings_from_fb)) {
            if (system_settings.updateSettings(system_settings_from_fb)) {
                system_settings.print();
                if (flags.sd_flag) {
                    updateSDSettings(system_settings);
                }
            }
        }
        updateFirebaseLocalIP(&firebaseData, WiFi.localIP().toString());
        setupWifiServer();
        setupMsgServer();
        setupTime();
        // createDir(SD_MMC,"/images");
    }
    if (!flags.camera_flag) {
        flags.camera_flag = setupCamera();
        if (!flags.camera_flag) {
            xTaskCreate(playNoCameraDetectedAsTask, "playNoCameraDetectedAsTask", STACK_SIZE, &mp3, 2, nullptr);
        }
    }

    if (!flags.wifi_flag && !flags.sd_flag) {
        xTaskCreate(playNoSDAndWifiAsTask, "playNoSDAndWifiAsTask", STACK_SIZE, &mp3, 2, nullptr);
    }
    Serial.printf("--------- System Init Done ---------\n");
}

void setup() {

    static int DistanceSensorDelay = 50;
    static int SpeedCalcDelay = 100;
    delay(5000);
    //xTaskCreate(playPoweringOnSystemAsTask, "playPoweringOnSystemAsTask", STACK_SIZE, &mp3, 2, nullptr);
    Serial.begin(115200);
    delay(100);
    Wire.begin(3, 14);
    Wire.setClock(100000); // Set I2C clock speed to 100 kHz
    delay(100);

   
    systemInit();

    


  
  is_system_on = true;
  String log_data = "SAFE STEP IS READY TO USE: STARTING OPERATIONS";
  logData(log_data);

 
}

void loop()
{
    if (flags.wifi_flag) {
        wifiServerLoop();
        msgServerLoop();
    }
}