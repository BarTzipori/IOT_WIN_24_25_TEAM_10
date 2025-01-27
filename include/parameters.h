
//SAFESTEP system defines

#define XSHUT_PIN_1 1
#define XSHUT_PIN_2 2
#define XSHUT_PIN_3 41
#define XSHUT_PIN_4 42
#define STACK_SIZE 4096
#define MPU9250_ADDRESS 0x68
#define VL53L1X_ADDRESS 0x54
#define VL53L1X_ADDRESS_2 0x55
#define VL53L1X_ADDRESS_3 0x52
#define VL53L1X_ADDRESS_4 0x53
#define MOTOR_1_PIN 19
#define MP3_RX 45
#define MP3_TX 48
#define ON_OFF_BUTTON_PIN 46
#define VOICE_ALERTS_DIR 0x06
#define ALERTS_DIR 0x07
#define LONG_PRESS_THRESHOLD 10000  // 10 second for long press
#define DOUBLE_PRESS_THRESHOLD 500 // 500 ms for double press

#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

// Define the camera configuration
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

#define SENSOR_1_ANGLE 15
#define SENSOR_2_ANGLE 30
#define SENSOR_3_ANGLE 15
#define SENSOR_4_ANGLE 30


//MP3 volume levels
#define VOLUME_1 0x10
#define VOLUME_2 0x20
#define VOLUME_3 0x30
#define VOLUME_4 0x40
#define VOLUME_5 0x50

//voice alerts indexes

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

//notification sounds indexes

#define COLLISION_WARNING_HEBREW 0x01
#define ALARM_CLOCK_4_BEEPS 0x02
#define ALERT_1 0x03
#define ALERT_2 0x04
#define BEEP_BEEP 0x05
#define WARNING_BEEPS 0x06
#define NOTIFICATION_SOUND 0x07
#define BEEP_BEEP_BEEP 0x08

//sensor positions according to box (height from bottom of box)

#define SENSOR_1_BOX_HEIGHT 0
#define SENSOR_2_BOX_HEIGHT 0
#define SENSOR_3_BOX_HEIGHT 0
#define SENSOR_4_BOX_HEIGHT 0




