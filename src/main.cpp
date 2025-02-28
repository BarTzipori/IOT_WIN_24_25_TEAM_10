
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
    String log_Data = "INFO: SYSTEM INITIALIZATION STARTED";
    logData(log_Data);
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
    system_settings.changeVolume(system_settings.getVolume(), &mp3); //updates the system volume
    log_Data = "INFO: SYSTEM INITIALIZATION DONE";  
    logData(log_Data);
}

void setup() {

    static int DistanceSensorDelay = 50;
    static int SpeedCalcDelay = 100;
    delay(5000);
    xTaskCreate(playPoweringOnSystemAsTask, "playPoweringOnSystemAsTask", STACK_SIZE, &mp3, 2, nullptr);
    Serial.begin(115200);
    delay(100);
    Wire.begin(3, 14);
    Wire.setClock(100000); // Set I2C clock speed to 100 kHz
    delay(100);

    while (!Serial)
        delay(10);
    // Initialize XSHUT pins
    for (size_t i = 0; i < distance_sensors_xshut_pins.size(); i++)
    {
        pinMode(distance_sensors_xshut_pins[i], OUTPUT);
    }
    // Initialize Distance measuring sensors
    bool distance_sensor_1_connected = initializeVL53L1XSensor(distance_sensors[0].first, XSHUT_PIN_1, distance_sensors[0].second, &Wire);
    bool distance_sensor_2_connected = initializeVL53L1XSensor(distance_sensors[1].first, XSHUT_PIN_2, distance_sensors[1].second, &Wire);
    bool distance_sensor_3_connected = initializeVL53L1XSensor(distance_sensors[2].first, XSHUT_PIN_3, distance_sensors[2].second, &Wire);
    bool distance_sensor_4_connected = initializeVL53L1XSensor(distance_sensors[3].first, XSHUT_PIN_4, distance_sensors[3].second, &Wire);
    bool all_distance_sensors_not_connected_flag = false;

    MPU9250Setting mpu_setting;
    mpu_setting.accel_fs_sel = ACCEL_FS_SEL::A2G;
    mpu_setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
    mpu_setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
    mpu_setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
    mpu_setting.gyro_fchoice = 0x03;
    mpu_setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_5HZ;
    mpu_setting.accel_fchoice = 0x01;
    mpu_setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_5HZ;

    systemInit();

    delay(2000);

    //check if some or all distance measuring sensors are not connected (on power up)
    if(!distance_sensor_1_connected && !distance_sensor_2_connected && !distance_sensor_3_connected && !distance_sensor_4_connected) {
        mp3.playWithFileName(VOICE_ALERTS_DIR, ALL_DISTANCE_MEARUSING_SENSORS_NOT_CONNECTED);
        delay(4000);
        all_distance_sensors_not_connected_flag = true;
    }
    if((!distance_sensor_1_connected || !distance_sensor_2_connected || !distance_sensor_3_connected || !distance_sensor_4_connected)&& !all_distance_sensors_not_connected_flag) {
        mp3.playWithFileName(VOICE_ALERTS_DIR, DISTANCE_SENSOR_DEGRADED );
        delay(7000);     
    }
    
    // Initializes MPU
    if (!mpu.setup(MPU9250_ADDRESS, mpu_setting)) {
        String log_data = "ERROR: MPU NOT DETECTED - SYSTEM WILL OPERATRE IN DEGRADED MODE";
        logData(log_data);
        xTaskCreate(playMPUSensorDegradedAsTask, "playMPUSensorDegradedAsTask", STACK_SIZE, &mp3, 2, nullptr);
        mpu_degraded_flag = true;
    } else {
        calibrateMPU(&mpu, calibration_needed, &mp3);
    } if (calibration_needed){
        delay(10000);
    }
    system_calibrated = true;

    bool buttonPressed = false;
    onOffButton.loop(); // Update button state

  if (flags.wifi_flag) {
    // Non-blocking delay for audio playback
    unsigned long audioStartTime = millis();
    xTaskCreate(playUploadLogsAsTask, "playUploadLogsAsTask", STACK_SIZE, &mp3, 2, nullptr);
    while (millis() - audioStartTime < 8000) {
      onOffButton.loop(); // Ensure button state is updated during audio playback
      if (onOffButton.isPressed()) {
        buttonPressed = true;
        break;
      } 
    }
    unsigned long startTime = millis();
    if(!buttonPressed) {
      // Prompt user to press the button within the timeout
      Serial.println("Press the button within the timeout (4 seconds) to upload logs.");
      while (millis() - startTime < UPLOAD_TIMEOUT) {
        onOffButton.loop(); // Update button state
        if (onOffButton.isPressed()) {
          buttonPressed = true;
          break; // Exit the loop if the button is pressed
        }
      }
    }
    // Check button press result
    if (buttonPressed) {
      String log_data = "INFO: Action button pressed. Uploading logs...";
      logData(log_data);
      xTaskCreate(playUploadingFilesAsTask, "playUploadingFilesAsTask", STACK_SIZE, &mp3, 2, nullptr);

      // Non-blocking delay for audio playback
      audioStartTime = millis();
      while (millis() - audioStartTime < 5000) {
        onOffButton.loop(); // Update button state during audio playback
      }
      // Upload logs and images
      uploadLogs(SD_MMC, fbdo, auth, config);   // Upload logs to Firebase
      uploadImages(SD_MMC, fbdo, auth, config); // Upload images to Firebase
    } else {
      String log_data = "INFO: Action button not pressed. Skipping log upload.";
      logData(log_data);
    }
    // Continue with the rest of the setup routine
  }
  xTaskCreate(playSystemReadytoUseAsTask, "playSystemReadytoUseAsTask", STACK_SIZE, &mp3, 2, nullptr);
  is_system_on = true;
  String log_data = "SAFE STEP IS READY TO USE: STARTING OPERATIONS";
  logData(log_data);

  // Creates threaded tasks
  VelocityTaskParams velocity_calc_params = {SpeedCalcDelay, &system_settings, &velocity, &step_count, &sensor_data, &is_system_on};
  SampleSensorDataParams sensor_data_params = { DistanceSensorDelay, &system_settings, &sensor_data, distance_sensors, mpu_sensors, &mp3, &is_system_on, &mpu_degraded_flag};

  xTaskCreate(sampleSensorsData, "sampleSensorsData", STACK_SIZE, &sensor_data_params, 2, nullptr);
  xTaskCreate(calculateVelocityAsTask, "calculateVelocity", STACK_SIZE, &velocity_calc_params, 3, nullptr);
}

void loop()
{
    if (flags.wifi_flag) {
        wifiServerLoop();
        msgServerLoop();
    }
    onOffButton.loop(); // Update button state

    static bool is_double_press_pending = false;      // Flag to track potential double press
    static unsigned long double_press_start_time = 0; // Tracks time of the first press in a double press

    // Press detection
    if (onOffButton.isPressed()) {
        pressed_time = millis();
        is_pressing = true;
        is_long_press = false;
    }

    // Release detection
    if (onOffButton.isReleased()) {
        is_pressing = false;
        unsigned long pressDuration = millis() - pressed_time; // Calculate press duration
        unsigned long currentTime = millis();

        if (pressDuration >= LONG_PRESS_THRESHOLD) {
            // Long press detected
            String log_data = "INFO: SAFESTEP MPU RECALIBRATION ROUTINE STARTING...";
            logData(log_data);
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
        } else if (pressDuration >= MEDIUM_PRESS_TRESHOLD && pressDuration < LONG_PRESS_THRESHOLD) {
            String log_data = "INFO: SAFESTEP MEDIUM PRESS ROUTINE STARTING...";
            logData(log_data);
            // Reset double press tracking
            is_double_press_pending = false;
            is_system_on = false;
            xTaskCreate(playErrorReportedAsTask, "playErrorReportedAsTask", STACK_SIZE, &mp3, 2, nullptr);
            String log_Data = "ERROR: System Malfunction reported by the user";
            logData(log_Data);
            CaptureError();
            is_system_on = true;
        } else {
            if (is_double_press_pending) {
                // Confirmed double press
                xTaskCreate(playWifiPairingInitiatedAsTask, "playWifiPairingInitiatedAsTask", STACK_SIZE, &mp3, 2, nullptr);
                vTaskDelay(5000);
                xTaskCreate(playPleaseConnectToSafestepWifiAsTask, "playPleaseConnectToSafestepWifiAsTask", STACK_SIZE, &mp3, 2, nullptr);
                String log_data = "INFO: SAFESTEP PAIRING PROCEDURE STARTED - PAIRING TO A NEW WIFI NETWORK...";
                logData(log_data);
                motor1.vibrate(vibrationPattern::pulseBuzz);
                if (!WifiManagerSetup()) {
                    Serial.println("Failed to connect to a new network, using SD card settings instead...");
                    String log_data = "ERROR: Failed to connect to a new network, using SD card settings instead...";
                    logData(log_data);
                    xTaskCreate(playSystemNotPairedAsTask, "playSystemNotPairedAsTask", STACK_SIZE, &mp3, 2, nullptr);
                    vTaskDelay(500);
                } else {
                    flags.wifi_flag = true;
                    systemInit();
                    xTaskCreate(playSystemPairedAsTask, "playSystemPairedAsTask", STACK_SIZE, &mp3, 2, nullptr);
                    vTaskDelay(500);
                    String log_data = "INFO: system paired to a new network";
                    logData(log_data);
                } 
                is_double_press_pending = false;
            } else {
                // First press of a potential double press
                is_double_press_pending = true;
                double_press_start_time = currentTime;
            }
        }
    }

    // Check for short press after double press timeout
    if (is_double_press_pending && (millis() - double_press_start_time > DOUBLE_PRESS_THRESHOLD)) {
        // No second press detected, treat as a short press
        String log_data = "INFO: SAFESTEP SHORT PRESS ROUTINE STARTING...";
        logData(log_data);
        motor1.vibrate(vibrationPattern::shortBuzz);

        // Toggle system mode
        String curr_mode = system_settings.getMode();
        if (curr_mode == "Both" || curr_mode == "Sound") {
            system_settings.setMode("Vibration");
            xTaskCreate(playSilentModeEnabledAsTask, "playSilentModeEnabledAsTask", STACK_SIZE, &mp3, 2, nullptr);
        } else {
            system_settings.setMode("Both");
            xTaskCreate(playSilentModeDisabledAsTask, "playSilentModeDisabledAsTask", STACK_SIZE, &mp3, 2, nullptr);
        }
        // Reset double press tracking
        is_double_press_pending = false;
    }
    if (is_system_on && !is_pressing && system_calibrated) {
        // Declare static variables to track last call time and previous measurements
        static unsigned long lastCollisionAlertTime = 0;
        static double previous_obstacle_collision_time = -1.0; // Initialize with an invalid value
        static double previous_obstacle_distance = -1.0; // Initialize with an invalid value

        if (system_settings.getAlertMethod() == "TimeToImpact") {
            if (mpu.update() && system_calibrated && is_system_on && !is_pressing) {
                double nearest_obstacle_collision_time = nearestObstacleCollisionTime(sensor_data, system_settings, &velocity);

                // Only trigger an alert if time to impact is decreasing (moving toward the obstacle)
                if (previous_obstacle_collision_time < 0 || 
                    nearest_obstacle_collision_time < previous_obstacle_collision_time) {
                    
                    if (millis() - lastCollisionAlertTime >= 50) {
                        if (collisionTimeAlertHandler(nearest_obstacle_collision_time, system_settings, mp3, motor1)) {
                            if (system_settings.getEnableCamera()) {
                                CaptureObstacle(fbdo, auth, config, flags.wifi_flag);
                            }
                        }
                        lastCollisionAlertTime = millis();
                    }
                }
                previous_obstacle_collision_time = nearest_obstacle_collision_time; // Update previous value
            }
        } else {
            if (is_system_on && !is_pressing) {
                double nearest_obstacle_distance = distanceToNearestObstacle(sensor_data, system_settings, &velocity, mpu_degraded_flag);

                // Only trigger an alert if the distance is decreasing (moving toward the obstacle)
                if (previous_obstacle_distance < 0 || 
                    nearest_obstacle_distance < previous_obstacle_distance) {
                    
                    if (millis() - lastCollisionAlertTime >= 50) {
                        if (obstacleDistanceAlertHandler(nearest_obstacle_distance, system_settings, mp3, motor1)) {
                            if (system_settings.getEnableCamera()) {
                                CaptureObstacle(fbdo, auth, config, flags.wifi_flag);
                            }
                        }
                        lastCollisionAlertTime = millis();
                    }
                }
                previous_obstacle_distance = nearest_obstacle_distance; // Update previous value
            }
        }
    }
    if (mpu_degraded_flag) {
        system_settings.setAlertMethod("Distance");
    }
    vTaskDelay(50);
}