#include "esp_camera.h"
#include <FS.h>       // File system support
#include <SD_MMC.h>   // SD_MMC library for ESP32

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

#define SD_MMC_CMD 38 //Please do not modify it.
#define SD_MMC_CLK 39 //Please do not modify it. 
#define SD_MMC_D0  40 //Please do not modify it.

bool card_mounted = true;

void setup() {
  Serial.begin(115200);
  delay(15000); // Allow time for the serial monitor to connect
  Serial.println("Initializing camera...");

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;  // Updated pin name
  config.pin_sccb_scl = SIOC_GPIO_NUM;  // Updated pin name
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

    // Initialize the camera
    if (esp_camera_init(&config) != ESP_OK) {
      Serial.println("Camera initialization failed!");
      while (true);
    }
    Serial.println("Camera initialized successfully.");

    // Initialize SD_MMC
    Serial.println("Initializing SD card...");
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
      card_mounted = false;
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
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

    Serial.println("SD card initialized successfully.");
}

static int photo_count = 0;  // Counter to make filenames unique

void loop() {
  if(card_mounted) {
  Serial.println("Capturing image...");

  // Capture a photo
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed!");
    return;
  }

  // Save the photo to the SD card
  String path = "/camera_photos/photo_" + String(photo_count) + ".jpg";
  photo_count++;  // Increment the counter
  Serial.printf("Saving image to: %s\n", path.c_str());
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing.");
    esp_camera_fb_return(fb);
    return;
  }

  file.write(fb->buf, fb->len); // Write the image buffer to the file
  file.close();
  esp_camera_fb_return(fb);

  Serial.println("Image saved successfully!");
  delay(5000); // Wait before capturing the next image
  } else {
    Serial.println("SD card not mounted. Please insert an SD card.");
    delay(5000);
  }
}