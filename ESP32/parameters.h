//user defined parameters

// OLED display width and height, in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//  4x3 keypad GPIO pins
#define R4   19 
#define R3   13 
#define R2   12 
#define R1   4    
#define C1   21 
#define C2   27 
#define C3   33  

// INMP441 I2S microphone GPIO pins
#define I2S_WS 14
#define I2S_SD 15
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

// external DAC MAX98357A GPIO pins
#define DAC_BCK_PIN 26
#define DAC_WS_PIN 25
#define DAC_DATA_PIN 22


//SAFESTEP system defines

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

