
//SAFESTEP system defines

#define XSHUT_PIN_1 1 // distance sensor 1 xshut pin
#define XSHUT_PIN_2 2 // distance sensor 2 xshut pin
#define XSHUT_PIN_3 41 // distance sensor 3 xshut pin
#define XSHUT_PIN_4 42 // distance sensor 4 xshut pin
#define VL53L1X_ADDRESS 0x54 // distance sensor 1 address
#define VL53L1X_ADDRESS_2 0x55 // distance sensor 2 address
#define VL53L1X_ADDRESS_3 0x52 // distance sensor 3 address
#define VL53L1X_ADDRESS_4 0x53 // distance sensor 4 address

#define STACK_SIZE 4096 // stack size for tasks
#define MPU9250_ADDRESS 0x68 // MPU9250 address

#define MOTOR_1_PIN 19 // vibration motor pin

#define MP3_RX 45 // mp3 player rx pin
#define MP3_TX 48 // mp3 player tx pin

#define ON_OFF_BUTTON_PIN 46 // on off button pin

#define VOICE_ALERTS_DIR 0x06 // voice alerts directory
#define ALERTS_DIR 0x07 // alerts directory

#define LONG_PRESS_THRESHOLD 10000  // 10 second for long press
#define MEDIUM_PRESS_TRESHOLD 800  // 2 second for medium press
#define DOUBLE_PRESS_THRESHOLD 500 // 500 ms for double press
#define UPLOAD_TIMEOUT 10000 // 10 seconds for upload timeout

// SD card pins. Those pins are used by the on board memory card. Please do not change this or use those pins for any other purpose.
#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

// Define the camera configuration - DO NOT TOUCH THIS, this is the default configuration for the camera. 
// Those pins are used by the camera and should not be used for any other type of hardware
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5
#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM       8
#define Y3_GPIO_NUM       9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

//MP3 volume levels (indexes of different volum levels)
#define VOLUME_1 0x10
#define VOLUME_2 0x20
#define VOLUME_3 0x30
#define VOLUME_4 0x40
#define VOLUME_5 0x50

//voice alerts indexes - these are the indexes used as input for the mp3 player (it needs an index to play a sound)

#define POWERING_ON_SYSTEM 0x04
#define SYSTEM_READY_TO_USE 0x01
#define SILENT_MODE_ACTIVATED 0x02
#define SILENT_MODE_DEACTIVATED 0x03
#define WIFI_CONNECTED 0x05
#define WIFI_NOT_CONNECTED 0x06
#define WIFI_PAIRING_INITIATED 0x07
#define NO_SD_DETECTED 0x08
#define NO_SD_AND_WIFI 0x09
#define NO_CAMERA_DETECTED 0x0A
#define MPU_CALIBRATION_START 0x0B
#define GYRO_CALIBRATION_START 0x0C
#define MAG_CALIBRATION_START 0x0D
#define MPU_CALIBRATION_DONE 0x0E
#define MPU_SENSOR_DEGRADED 0x0F
#define DISTANCE_SENSOR_DEGRADED 0x10
#define UPLOAD_LOGS 0x11
#define UPLOADING_FILES 0x12
#define ERROR_REPORTED 0x13
#define PLEASE_CONNECT_TO_SAFESTEP_WIFI 0x14
#define SYSTEM_PAIRED 0x15
#define SYSTEM_NOT_PAIRED 0x16
#define ALL_DISTANCE_MEARUSING_SENSORS_NOT_CONNECTED 0x17

//notification sounds indexes - these are the indexes used as input for the mp3 player (it needs an index to play a sound)
#define COLLISION_WARNING_HEBREW 0x01
#define ALARM_CLOCK_4_BEEPS 0x02
#define ALERT_1 0x03
#define ALERT_2 0x04
#define BEEP_BEEP 0x05
#define WARNING_BEEPS 0x06
#define NOTIFICATION_SOUND 0x07
#define BEEP_BEEP_BEEP 0x08

//sensor positions according to box (height from bottom of box)

#define SENSOR_1_ANGLE 60 //angle from the horizontal axis
#define SENSOR_2_ANGLE 45 //angle from the horizontal axis
#define SENSOR_3_ANGLE 30 //angle from the horizontal axis
#define SENSOR_4_ANGLE 15 //angle from the horizontal axis

#define SENSOR_1_BOX_HEIGHT 50 //Height from the bottom of the box
#define SENSOR_2_BOX_HEIGHT 40 //Height from the bottom of the box
#define SENSOR_3_BOX_HEIGHT 25 //Height from the bottom of the box
#define SENSOR_4_BOX_HEIGHT 10 //Height from the bottom of the box

//additional system settings (comms, wifi, camera)

#define WIFI_MANAGER_TIMEOUT 30 // Timeout for wifi manager in seconds (after which the system will continue without pairing to a new wifi network)
#define WIFI_TIMEOUT 15000 // Timeout for wifi connection in milliseconds (after which the system will carry on without wifi)
#define CAMERA_CAPTURE_INTERVAL 5000 // Camera capture interval in milliseconds
#define WIFI_AP_SSID "SafeStepAP" // wifi AP SSID for the safestep network
#define WIFI_AP_PASSWORD "safestep2025" // wifi AP password for the safestep network
//TOD settings
#define GMT_OFFSET 7200
#define DAYLIGHT_OFFSET 3600

#define SERVER_RESPONE_TIMEOUT 10000 // Timeout for server response in milliseconds
#define SERVER_SOCKET_TIMEOUT 15000 // Timeout for server socket in milliseconds
#define APP_SERVER_PORT 80 // Port for the app server
#define WEBMSG_SERVER_PORT 82 // Port for the webmsg server

//collision detection algorithm settings 
#define STEP_TIME_THRESHOLD 300 // Minimum time between steps in milliseconds
#define SPEED_WINDOW_MS 1000 // Speed calculation window in milliseconds
// Thresholds for step detection
#define STEP_HIGH_THRESHOLD 0.07f  // Threshold for detecting a step's peak in AccX
#define STEP_LOW_THRESHOLD 0.02f   // Threshold for resetting the step state
#define GYRO_THRESHOLD 0.08f       // Minimum gyroscope change to indicate a step
#define DISTANCE_CHANGE_THRESHOLD 200 // Threshold in mm to consider significant movement
#define ESTIMATED_STRIDE_LENGTH 0.415 // Estimated stride length in meters


